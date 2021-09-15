#pragma once

//#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "IPeriodicChannelEx.h"
#include "TckChannelBase.h"
#include "tickCounters.h"
//#include "tck.h"

namespace TeensyTimerTool
{
    //#if !defined(ARDUINO_TEENSYLC) // T-LC doesn't have a cycle counter, needs special treatment

    template <typename TickCounter>
    class TckPeriodicChannel : public IPeriodicChannelEx
    {
     public:
        TckPeriodicChannel(callback_t *cbStorage);
        ~TckPeriodicChannel() { end(); }
        errorCode begin(callback_t cb, float period, bool startImmediately) override;
        errorCode start() override;
        errorCode stop() override;
        errorCode end() override;
        errorCode setPeriod(float us) override { return errorCode::notImplemented; }

        float getMaxPeriod() override { return 0; }

     protected:
        using counter_t = typename TickCounter::counter_t; // use type from the tickCounter (uint32_t or uint64_t)

        void tick() override;

        callback_t callback;
        bool periodic;

        counter_t startCnt, periodCnt, nextPeriod, period;
    };

    // INLINE IMPLEMENTATION ==============================================

    template <typename period_t>
    TckPeriodicChannel<period_t>::TckPeriodicChannel(callback_t *cbStorage)
    {
        //currentPeriod = 0;
        Serial.println("tckPeriodicChannel ctor");
    }

    template <typename TickCounter>
    errorCode TckPeriodicChannel<TickCounter>::begin(callback_t cb, float period, bool startImmediately)
    {
        this->period   = TickCounter::us2ticks(period);
        this->callback = cb;

        if (startImmediately) start();
        return errorCode::OK;
    }

    template <typename TickCounter>
    errorCode TckPeriodicChannel<TickCounter>::start()
    {
        periodCnt  = period;
        nextPeriod = period;
        startCnt   = TickCounter::getCount();

        return errorCode::OK;
    }

    template <typename period_t>
    errorCode TckPeriodicChannel<period_t>::stop()
    {
        periodCnt = 0;
        return errorCode::OK;
    }

    template <typename TickCounter>
    errorCode TckPeriodicChannel<TickCounter>::end()
    {
        extern void removeTimer(ITimerChannelEx * channel);
        stop();
        removeTimer(this);
        return errorCode::OK;
    }

    template <typename TickCounter>
    void TckPeriodicChannel<TickCounter>::tick()
    {
        if (periodCnt == 0) return;

        static bool lock = false;
        counter_t now    = TickCounter::getCount();
        if (!lock && (now - startCnt) >= periodCnt)
        {
            lock = true;
            callback();
            startCnt += periodCnt;
            lock = false;
        }
    }

    // template <typename ct>
    // uint32_t TckChannel<ct>::CPUCyclesToMicroseond(const uint32_t cpuCycles)
    // {
    //     return (1'000'000.0f / F_CPU) * cpuCycles;
    // }

    // template <typename ct>
    // void TckChannel<ct>::_setNextPeriod(const uint32_t period)
    // {
    //     this->nextPeriod = period;
    // }

    // template <typename ct>
    // errorCode TckChannel<ct>::_setCurrentPeriod(const uint32_t period)
    // {
    //     this->currentPeriod = period;
    //     const bool hasTicked = this->tick();

    //     if (hasTicked)
    //     {
    //         return errorCode::triggeredLate;
    //     } else
    //     {
    //         return errorCode::OK;
    //     }
    // }

    // template <typename ct>
    // errorCode TckChannel<ct>::setPeriod(uint32_t microSeconds)
    // {
    //     const uint32_t period = microsecondToCPUCycles(microSeconds);

    //     this->_setNextPeriod(period);
    //     return this->_setCurrentPeriod(period);
    // }

    // template <typename ct>
    // errorCode TckChannel<ct>::setPeriod(uint32_t microSeconds)
    // {
    //     const uint32_t period = microsecondToCPUCycles(microSeconds);
    //     return this->_setCurrentPeriod(period);
    // }

    // template <typename ct>
    // errorCode TckChannel<ct>::setNextPeriod(uint32_t microSeconds)
    // {
    //     const uint32_t period = microsecondToCPUCycles(microSeconds);
    //     this->_setNextPeriod(period);
    //     return errorCode::OK;
    // }

    // #else // TeensyLC

    //     // Quick hack only, needs to be improved

    //     template <typename uint32_64_t>
    //     class TckChannel : public TckChannelBase
    //     {
    //      public:
    //         TckChannel() { triggered = false; }
    //         virtual ~TckChannel(){};

    //         errorCode begin(callback_t cb, float period, bool periodic) override
    //         {
    //             triggered      = false;
    //             this->periodic = periodic;
    //             this->period   = (uint32_t)period;
    //             this->callback = cb;

    //             return errorCode::OK;
    //         }

    //         errorCode start()
    //         {
    //             this->startCNT  = micros();
    //             this->triggered = true;
    //             return errorCode::OK;
    //         }

    //         errorCode stop()
    //         {
    //             this->triggered = false;
    //             return errorCode::OK;
    //         }

    //         inline errorCode setPeriod(uint32_t microSeconds) override;
    //         inline uint32_t getPeriod(void);

    //         inline errorCode trigger(float delay) // µs
    //         {
    //             this->startCNT  = micros();
    //             this->period    = (uint32_t)delay;
    //             this->triggered = true;
    //             return errorCode::OK;
    //         }

    //         float getMaxPeriod() const override { return getMaxMicros() / 1E6; } // seconds

    //      protected:
    //         static constexpr float clock = F_CPU / 1'000'000.0f;
    //         float getMaxMicros() const { return 0xF000'0000 / clock; } // don't use full range otherwise tick might miss the turnover for large periods

    //         uint32_t startCNT, period;
    //         callback_t callback;
    //         bool triggered;
    //         bool periodic;

    //         inline bool tick();
    //         bool block = false;

    //         friend TCK_t;
    //     };

    //     // IMPLEMENTATION ==============================================

    //     template <typename ct>
    //     bool TckChannel<ct>::tick()
    //     {
    //         static bool lock = false;

    //         if (!lock && period != 0 && triggered && (micros() - startCNT) >= period)
    //         {
    //             lock      = true;
    //             startCNT  = micros();
    //             triggered = periodic; // i.e., stays triggerd if periodic, stops if oneShot
    //             callback();
    //             lock = false;
    //             return true;
    //         }
    //         return false;
    //     }

    //     template <typename ct>
    //     errorCode TckChannel<ct>::setPeriod(uint32_t microSeconds)
    //     {
    //         period = microSeconds;
    //         return errorCode::OK;
    //     }

    //     template <typename ct>
    //     uint32_t TckChannel<ct>::getPeriod()
    //     {
    //         return period;
    //     }

    // #endif

} // namespace TeensyTimerTool
