#pragma once

//#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "TckChannelBase.h"

namespace TeensyTimerTool
{
    template <typename tckCounter> // tckCounter is the underlying counter, e.g. 32bit cycle counter, RTC counter, micros counter etc
    class TckChannel : public TckChannelBase
    {
        using counter_t = typename tckCounter::counter_t; // use the counter type (uint32_t, unit64_t...) of the used tckCounter class

     public:
        TckChannel();

        inline errorCode begin(callback_t cb, float period, bool periodic) override;
        inline errorCode start() override;
        inline errorCode stop() override;

        inline errorCode trigger(float delay_us) override;
        inline errorCode triggerDirect(counter_t reload) override;
        inline errorCode getTriggerReload(float delay, counter_t *reload) override;

        inline errorCode setNextPeriod(float us) override;
        inline errorCode setPeriod(float us) override;
        inline errorCode setPeriodKeepElapsed(float us) override;

        float getMaxPeriod() const override { return tckCounter::getMaxMicros() / 1E6f; } // seconds
        float getRemainingTime() const override // seconds
        {
            if (!triggered) return 0; // stopped, one-shot fired or never triggered
            counter_t elapsed = tckCounter::getCount() - startCnt;
            if (elapsed >= currentPeriod) return 0;
            return (currentPeriod - elapsed) / (float)tckCounter::us2ticks(1E6f); // ticks per second of the used counter (was F_CPU for all)
        }

        double getPeriod() const override { return (double)nextPeriod / tckCounter::us2ticks(1E6f); } // latest period set
        double getResolution() const override { return 1.0 / tckCounter::us2ticks(1E6f); }

     protected:
        inline bool tick();
        inline errorCode us2ticks(float us, counter_t *ticks) const; // clamps to the max period, at least one tick
        callback_t callback;
        counter_t startCnt, currentPeriod, nextPeriod;

        bool periodic;
        bool triggered;
    };

    // IMPLEMENTATION ==============================================

    template <typename tckCounter>
    TckChannel<tckCounter>::TckChannel()
    {
        triggered = false;
    }

    template <typename tckCounter>
    errorCode TckChannel<tckCounter>::begin(callback_t cb, float period, bool periodic)
    {
        this->triggered = false;

        this->periodic   = periodic;
        errorCode status = errorCode::OK;
        if (periodic)
        {
            status           = us2ticks(period, &this->currentPeriod);
            this->nextPeriod = this->currentPeriod;
        }
        this->callback = cb;

        return status;
    }

    template <typename tckCounter>
    errorCode TckChannel<tckCounter>::start() // the first period is the latest one set
    {
        this->currentPeriod = this->nextPeriod;
        this->startCnt  = tckCounter::getCount();
        this->triggered = true;
        return errorCode::OK;
    }

    template <typename tckCounter>
    errorCode TckChannel<tckCounter>::stop()
    {
        this->triggered = false;
        return errorCode::OK;
    }

    template <typename tckCounter>
    errorCode TckChannel<tckCounter>::triggerDirect(counter_t reload)
    {
        this->startCnt      = tckCounter::getCount();
        this->nextPeriod    = reload;
        this->currentPeriod = this->nextPeriod;
        this->triggered     = true;
        return errorCode::OK;
    }

    template <typename tckCounter>
    errorCode TckChannel<tckCounter>::trigger(float delay) // µs
    {
        counter_t reload;
        errorCode status = us2ticks(delay, &reload);
        triggerDirect(reload);
        return status;
    }

    template <typename tckCounter>
    errorCode TckChannel<tckCounter>::getTriggerReload(float delay, counter_t *reload)
    {
        return us2ticks(delay, reload);
    }

    template <typename tckCounter>
    errorCode TckChannel<tckCounter>::setNextPeriod(float us)
    {
        return us2ticks(us, &nextPeriod);
    }

    template <typename tckCounter>
    errorCode TckChannel<tckCounter>::setPeriod(float us) // restart: the new period starts now
    {
        counter_t newPeriod;
        errorCode status = us2ticks(us, &newPeriod);
        startCnt         = tckCounter::getCount();
        currentPeriod    = newPeriod;
        nextPeriod       = newPeriod;
        return status;
    }

    template <typename tckCounter>
    errorCode TckChannel<tckCounter>::setPeriodKeepElapsed(float us) // behaviour of setPeriod() before v1.5
    {
        counter_t newPeriod;
        errorCode status = us2ticks(us, &newPeriod);
        if (!triggered) // stopped: nothing elapsed, start() uses the new period
        {
            currentPeriod = newPeriod;
            nextPeriod    = newPeriod;
            return status;
        }
        counter_t now = tckCounter::getCount();

        if (now - startCnt >= newPeriod) // new period already expired
        {                                //
            startCnt = now;              // -> restart cycle but,
            callback();                  // since expired, invoke callback now
        }                                //
        currentPeriod = newPeriod;       // in any case next callback will
        nextPeriod    = newPeriod;       // be invoked after newly set period
        return status;
    }

    template <typename tckCounter>
    errorCode TckChannel<tckCounter>::us2ticks(float us, counter_t *ticks) const
    {
        if (us > tckCounter::getMaxMicros())
        {
            *ticks = tckCounter::us2ticks(tckCounter::getMaxMicros());
            return errorCode::periodOverflow;
        }
        *ticks = tckCounter::us2ticks(us);
        if (*ticks == 0) *ticks = 1; // tick() never fires a zero period
        return errorCode::OK;
    }

    template <typename tckCounter>
    bool TckChannel<tckCounter>::tick()
    {
        static bool lock = false;
        counter_t now    = tckCounter::getCount();
        if (!lock && this->currentPeriod != 0 && this->triggered && (now - this->startCnt) >= this->currentPeriod)
        {
            lock = true;
            this->startCnt += currentPeriod;
            this->currentPeriod = nextPeriod;     // update period if it was changed during current cycle.
            this->triggered     = this->periodic; // i.e., stays triggerd if periodic, stops if oneShot
            callback();
            lock = false;
            return true;
        } else
        {
            return false;
        }
    }

} // namespace TeensyTimerTool
