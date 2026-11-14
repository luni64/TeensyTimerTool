#pragma once

#include "Arduino.h"
#include "ITimerChannel.h"

namespace TeensyTimerTool
{
    class PIT_t;

    class PITChannel final : public ITimerChannel // final: internal calls of virtual functions are direct calls
    {
     public:
        inline PITChannel(unsigned nr);
        inline virtual ~PITChannel();

        inline errorCode begin(callback_t cb, float tcnt, bool periodic) override;
        inline errorCode start() override;
        inline errorCode stop() override;

        inline errorCode trigger(float) override;
        inline errorCode triggerDirect(uint32_t reload) override;
        inline errorCode getTriggerReload(float delay, uint32_t *reload) override;
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
        inline void stopForTrigger();
        inline void startTrigger(uint32_t reload);
        float clock;
        // RAM copies of register values. PIT registers sit behind the slow peripheral bus, every access costs ~100 CPU
        // cycles (24 MHz timer clock), and a read additionally waits for all pending register writes. The ISR,
        // start() and trigger() therefore don't read registers. The copies are valid since only this class writes
        // LDVAL. Interrupt flags are never copied: they are always cleared by a register write (see PIT_t::isr).
        uint32_t ldval     = 0; // LDVAL, i.e. reload of the following periods
        uint32_t curReload = 0; // reload of the current period (LDVAL itself may already hold the next one)
        bool running       = false; // periodic timer started (TCTRL.TEN), avoids reading TCTRL

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
        clock = (CCM_CSCMR1 & CCM_CSCMR1_PERCLK_CLK_SEL) ? 24.0f : F_BUS_ACTUAL / 1E6f; // MHz, F_BUS needn't be whole MHz (e.g. 75.6)
        isPeriodic = periodic;
        callback   = cb;

        errorCode status = errorCode::OK;
        if (isPeriodic)
        {
            IMXRT_PIT_CHANNELS[chNr].TCTRL = 0;
            IMXRT_PIT_CHANNELS[chNr].TFLG  = 1;
            running                        = false;
            status                         = setNextPeriod(micros);
        }
        return status;
    }

    // reload = ticks - 1 (period is LDVAL + 1 ticks), rounded to the nearest tick, at least one tick.
    // Doesn't post errors, callers return the status which the API layer posts.
    // Hot path of trigger(): one multiply, no division (a float division costs 14 cycles, getMaxPeriod() has two)
    errorCode PITChannel::us2ticks(float micros, uint32_t *reload) const
    {
        float ticks = clock * micros + 0.5f;
        if (ticks >= 4294967296.0f) // 2^32: more than 0xFFFF'FFFF ticks (max reload 0xFFFF'FFFE)
        {
            *reload = 0xFFFF'FFFE;
            return errorCode::periodOverflow;
        }
        *reload     = ticks < 2.0f ? 0 : (uint32_t)ticks - 1;
        return errorCode::OK;
    }

    errorCode PITChannel::start()
    {
        curReload                      = ldval;
        IMXRT_PIT_CHANNELS[chNr].TCTRL = PIT_TCTRL_TEN | PIT_TCTRL_TIE; // TEN loads LDVAL: the first period is the latest one set
        running                        = true;
        return errorCode::OK;
    }

    errorCode PITChannel::stop()
    {
        IMXRT_PIT_CHANNELS[chNr].TCTRL = 0;
        IMXRT_PIT_CHANNELS[chNr].TFLG  = 1; // a flag set just before would call this callback when another channel fires
        running                        = false;
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

    errorCode PITChannel::setPeriod(float us) // a stopped timer stays stopped, start() uses the new period
    {
        if (!running) return setNextPeriod(us);
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
        if (!running)
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

    // trigger() / triggerDirect(): register writes only, the same writes as hand-written code. trigger() computes
    // the reload between the writes: the CPU stalls when a write to the slow peripheral bus is still pending, the
    // conversion runs in this time for free.
    errorCode PITChannel::trigger(float delay)
    {
        stopForTrigger();
        uint32_t reload;
        errorCode status = us2ticks(delay, &reload);
        startTrigger(reload);
        return status;
    }

    errorCode PITChannel::triggerDirect(uint32_t reload)
    {
        stopForTrigger();
        startTrigger(reload);
        return errorCode::OK;
    }

    void PITChannel::stopForTrigger()
    {
        IMXRT_PIT_CHANNELS[chNr].TCTRL = 0; // stop: a new LDVAL is loaded only at start (RM 53.9.5.4)
        IMXRT_PIT_CHANNELS[chNr].TFLG  = 1; // a flag of an expired previous trigger would fire at once
    }

    void PITChannel::startTrigger(uint32_t reload)
    {
        IMXRT_PIT_CHANNELS[chNr].LDVAL = reload;
        IMXRT_PIT_CHANNELS[chNr].TCTRL = PIT_TCTRL_TEN | PIT_TCTRL_TIE;
        ldval                          = reload;
        curReload                      = reload;
    }

    errorCode PITChannel::getTriggerReload(float delay, uint32_t *reload)
    {
        return us2ticks(delay, reload);
    }

    float PITChannel::getMaxPeriod() const
    {
        return (float)0xFFFF'FFFE / clock / 1'000'000;
    }

} // namespace TeensyTimerTool