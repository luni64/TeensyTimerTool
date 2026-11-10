#pragma once

#include "Arduino.h"
#include "ITimerChannel.h"

namespace TeensyTimerTool
{
    class PIT_t;

    class PITChannel : public ITimerChannel
    {
     public:
        inline PITChannel(unsigned nr);
        inline virtual ~PITChannel();

        inline errorCode begin(callback_t cb, float tcnt, bool periodic) override;
        inline errorCode start() override;
        inline errorCode stop() override;

        inline errorCode trigger(float) override;
        inline errorCode setNextPeriod(float us) override;
        inline errorCode setPeriod(float us) override;
        inline errorCode setPeriodKeepElapsed(float us) override;

        inline float getMaxPeriod() const override;
        double getPeriod() const override { return (ldval + 1.0) / (clock * 1E6); } // period = LDVAL + 1 ticks
        double getResolution() const override { return 1.0 / (clock * 1E6); }
        float getRemainingTime() const override // counts down CVAL..0; one-shots are disabled when they fire
        {
            if (!(IMXRT_PIT_CHANNELS[chNr].TCTRL & PIT_TCTRL_TEN)) return 0;
            return (IMXRT_PIT_CHANNELS[chNr].CVAL + 1.0f) / (clock * 1E6f);
        }

        inline void release() override; // channels are static members of PIT_t, stop and mark free instead of delete

        bool isPeriodic;

     protected:
        inline void isr();

        inline errorCode us2ticks(float micros, uint32_t *reload) const;
        float clock;
        // RAM copies of register values. PIT registers sit behind the slow peripheral bus, every access costs ~100 CPU
        // cycles (24 MHz timer clock), and a read additionally waits for all pending register writes. The ISR,
        // start() and trigger() therefore don't read registers. The copies are valid since only this class writes
        // LDVAL. Interrupt flags are never copied: they are always cleared by a register write (see PIT_t::isr).
        uint32_t ldval     = 0; // LDVAL, i.e. reload of the following periods
        uint32_t curReload = 0; // reload of the current period (LDVAL itself may already hold the next one)

        const unsigned chNr;
        callback_t callback = nullptr;

        friend PIT_t;
        PITChannel()                   = delete;
        PITChannel(const PITChannel &) = delete;
    };

    // IMPLEMENTATION ==============================================

    PITChannel::PITChannel(unsigned nr)
        : ITimerChannel(nullptr), chNr(nr)
    {
        callback = nullptr;        
    }

    errorCode PITChannel::begin(callback_t cb, float micros, bool periodic)
    {
        clock = (CCM_CSCMR1 & CCM_CSCMR1_PERCLK_CLK_SEL) ? 24 : (F_BUS_ACTUAL / 1000000);        
        isPeriodic = periodic;
        callback   = cb;

        errorCode status = errorCode::OK;
        if (isPeriodic)
        {
            IMXRT_PIT_CHANNELS[chNr].TCTRL = 0;
            IMXRT_PIT_CHANNELS[chNr].TFLG  = 1;
            status                         = setNextPeriod(micros);
        }
        return status;
    }

    // reload = ticks - 1 (period is LDVAL + 1 ticks), rounded to the nearest tick, at least one tick.
    // Doesn't post errors, callers return the status which the API layer posts.
    errorCode PITChannel::us2ticks(float micros, uint32_t *reload) const
    {
        // getMaxPeriod() returns seconds, micros is in µs
        if (micros > getMaxPeriod() * 1E6f)
        {
            *reload = 0xFFFF'FFFE;
            return errorCode::periodOverflow;
        }
        float ticks = clock * micros + 0.5f;
        *reload     = ticks < 2.0f ? 0 : (uint32_t)ticks - 1;
        return errorCode::OK;
    }

    errorCode PITChannel::start()
    {
        curReload                      = ldval;
        IMXRT_PIT_CHANNELS[chNr].TCTRL = PIT_TCTRL_TEN | PIT_TCTRL_TIE;
        return errorCode::OK;
    }

    errorCode PITChannel::stop()
    {
        IMXRT_PIT_CHANNELS[chNr].TCTRL = 0;
        IMXRT_PIT_CHANNELS[chNr].TFLG  = 1; // a flag set just before would call this callback when another channel fires
        return errorCode::OK;
    }

    errorCode PITChannel::setNextPeriod(float us)
    {
        uint32_t reload;
        errorCode status               = us2ticks(us, &reload);
        ldval                          = reload;
        IMXRT_PIT_CHANNELS[chNr].LDVAL = reload;
        return status;
    }

    errorCode PITChannel::setPeriod(float us)
    {
        stop(); // need to stop/start timer to change current period (see ch 53.9.5.4)
        errorCode status = setNextPeriod(us);
        start();
        return status;
    }

    errorCode PITChannel::setPeriodKeepElapsed(float us) // the elapsed time of the current period counts
    {
        uint32_t reload;
        errorCode status = us2ticks(us, &reload);
        auto &ch         = IMXRT_PIT_CHANNELS[chNr];
        if (!(ch.TCTRL & PIT_TCTRL_TEN))
        {
            ldval    = reload;
            ch.LDVAL = reload; // not running: start() starts the new period
            return status;
        }
        uint32_t elapsed = curReload - ch.CVAL;                // ticks since the period started
        uint32_t rest    = elapsed >= reload ? 0 : reload - elapsed; // already past: fire after one tick
        ch.TCTRL         = 0;
        ch.LDVAL         = rest;
        ch.TCTRL         = PIT_TCTRL_TEN | PIT_TCTRL_TIE; // loads LDVAL (the remainder)
        ch.LDVAL         = reload;                        // following periods
        ldval            = reload;
        curReload        = reload;                        // CVAL counts down from rest: elapsed = reload - CVAL
        return status;
    }

    void PITChannel::isr()
    {
        if (callback != nullptr)
        {
            if (!isPeriodic) // switch off before the callback: a short delay could expire again while the
            {                // callback runs, and the callback may re-trigger the timer
                IMXRT_PIT_CHANNELS[chNr].TCTRL = 0;
                IMXRT_PIT_CHANNELS[chNr].TFLG  = 1;
            }
            else
                curReload = ldval; // the hardware loaded LDVAL for the new period
            callback();
        }
    }

    PITChannel::~PITChannel()
    {
        callback = nullptr;
    }

    void PITChannel::release()
    {
        stop();
        callback = nullptr; // marks the channel as free (see PIT_t::getTimer)
    }

    errorCode PITChannel::trigger(float delay) // should be optimized somehow
    {
        IMXRT_PIT_CHANNELS[chNr].TCTRL = 0;
        IMXRT_PIT_CHANNELS[chNr].TFLG  = 1;

        errorCode status = setNextPeriod(delay);
        start();

        return status;
    }

    float PITChannel::getMaxPeriod() const
    {
        return (float)0xFFFF'FFFE / clock / 1'000'000;
    }

} // namespace TeensyTimerTool