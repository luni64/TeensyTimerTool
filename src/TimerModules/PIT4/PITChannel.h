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

        inline float getMaxPeriod() const override;
        double getPeriod() const override { return (IMXRT_PIT_CHANNELS[chNr].LDVAL + 1.0) / (clock * 1E6); } // period = LDVAL + 1 ticks
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
        IMXRT_PIT_CHANNELS[chNr].TCTRL = PIT_TCTRL_TEN | PIT_TCTRL_TIE;
        return errorCode::OK;
    }

    errorCode PITChannel::stop()
    {
        IMXRT_PIT_CHANNELS[chNr].TCTRL = 0;
        return errorCode::OK;
    }

    errorCode PITChannel::setNextPeriod(float us)
    {
        uint32_t reload;
        errorCode status               = us2ticks(us, &reload);
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

    void PITChannel::isr()
    {
        if (callback != nullptr)
        {
            if (!isPeriodic) // switch off before the callback: a short delay could expire again while the
            {                // callback runs, and the callback may re-trigger the timer
                IMXRT_PIT_CHANNELS[chNr].TCTRL = 0;
                IMXRT_PIT_CHANNELS[chNr].TFLG  = 1;
            }
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