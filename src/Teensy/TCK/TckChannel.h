#pragma once

#include "ErrorHandling/error_codes.h"
#include "TckChannelBase.h"

namespace TeensyTimerTool
{
    template <typename tickTimer> // tickTimer is the underlying counter, e.g. 32bit cycle counter, RTC counter, micros counter etc
    class TckChannel : public TckChannelBase
    {
        using counter_t = typename tickTimer::counter_t; // use counter type (uint32_t or unit64_t) defined in the used tickTimer

     public:
        TckChannel();

        inline errorCode begin(callback_t cb, float period, bool periodic) override;
        inline errorCode start() override;
        inline errorCode stop() override;

        inline errorCode trigger(float delay_us) override;
        inline errorCode triggerDirect(counter_t reload) override;
        inline errorCode getTriggerReload(float delay, counter_t* reload) override;

        float getMaxPeriod() const override { return tickTimer::getMaxMicros() / 1E6; } // seconds

     protected:
        inline bool tick();
        callback_t callback;
        counter_t startCnt, currentPeriod, nextPeriod;

        bool periodic;
        bool triggered;
    };

    // IMPLEMENTATION ==============================================

    template <typename tckTimer>
    TckChannel<tckTimer>::TckChannel()
    {
        triggered = false;
    }

    template <typename tckTimer>
    errorCode TckChannel<tckTimer>::begin(callback_t cb, float period, bool periodic)
    {
        this->triggered = false;

        this->periodic = periodic;
        if (periodic)
        {
            this->currentPeriod = tckTimer::us2ticks(period);
            this->nextPeriod = this->currentPeriod;
        }
        this->callback = cb;

        return errorCode::OK;
    }

    template <typename tckTimer>
    errorCode TckChannel<tckTimer>::start()
    {
        this->startCnt = tckTimer::getCount();
        this->triggered = true;
        return errorCode::OK;
    }

    template <typename tckTimer>
    errorCode TckChannel<tckTimer>::stop()
    {
        this->triggered = false;
        return errorCode::OK;
    }

    template <typename tckTimer>
    errorCode TckChannel<tckTimer>::triggerDirect(counter_t reload)
    {
        this->startCnt = tckTimer::getCount();
        this->nextPeriod = reload;
        this->currentPeriod = this->nextPeriod;
        this->triggered = true;
        return errorCode::OK;
    }

    template <typename tckTimer>
    errorCode TckChannel<tckTimer>::trigger(float delay) // µs
    {
        return triggerDirect(tckTimer::us2ticks(delay));
    }

    template <typename tckTimer>
    errorCode TckChannel<tckTimer>::getTriggerReload(float delay, counter_t* reload)
    {
        *reload = tckTimer::us2ticks(delay);
        return errorCode::OK;
    }

    template <typename tckTimer>
    bool TckChannel<tckTimer>::tick()
    {
        static bool lock = false;
        counter_t now = tckTimer::getCount();
        if (!lock && this->currentPeriod != 0 && this->triggered && (now - this->startCnt) >= this->currentPeriod)
        {
            lock = true;
            //this->startCnt = now;
            this->startCnt += currentPeriod;
            this->triggered = this->periodic; // i.e., stays triggerd if periodic, stops if oneShot
            callback();
            lock = false;
            return true;
        } else
        {
            return false;
        }
    }

} // namespace TeensyTimerTool
