#pragma once

//#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "TckChannelBase.h"
#include "core_pins.h"

namespace TeensyTimerTool
{
    class TCK_t;

#if !defined(ARDUINO_TEENSYLC) // T-LC doesn't have a cycle counter, nees special treatment

    template <typename CounterType>
    class TckChannel : public TckChannelBase
    {
     public:
        TckChannel();
        virtual ~TckChannel(){}; //TBD

        inline errorCode begin(callback_t cb, float period, bool periodic) override;
        inline errorCode start() override;
        inline errorCode stop() override;

        inline errorCode trigger(float delay_us) override;
        inline errorCode triggerDirect(CounterType reload) override;
        inline errorCode getTriggerReload(float delay, CounterType* reload) override
        {
            *reload = microsecondToCycles(delay);
            return errorCode::OK;
        }

        float getMaxPeriod() const override { return getMaxMicros() / 1E6; } // seconds

        CounterType getCycleCounter() { postError(errorCode::wrongType); }

        // inline errorCode setPeriod(uint32_t microSeconds) override;
        // inline errorCode setPeriod(uint32_t microSeconds) override;
        // inline errorCode setNextPeriod(uint32_t microSeconds) override;

     protected:
        inline CounterType microsecondToCycles(const float microSecond) const;
        float getMaxMicros() const;

        inline bool tick();
        callback_t callback;

        bool periodic;
        bool triggered;

        // bool block = false;

        CounterType startCnt, currentPeriod, nextPeriod;

        uint32_t lastCyCnt;
        uint32_t curHIGH;

        float clock;

        friend TCK_t;

        // inline uint32_t CPUCyclesToMicroseond(const uint32_t cpuCycles);
        // inline errorCode _setCurrentPeriod(const uint32_t period);
        // inline void _setNextPeriod(const uint32_t period);
    };

    // IMPLEMENTATION ==============================================

    template <typename T>
    TckChannel<T>::TckChannel()
    {
        triggered = false;
        clock = F_CPU / 1'000'000.0f;
    }

    template <typename T>
    errorCode TckChannel<T>::begin(callback_t cb, float period, bool periodic)
    {
        this->triggered = false;

        this->periodic = periodic;
        if (periodic)
        {
            this->currentPeriod = microsecondToCycles(period);
            this->nextPeriod = this->currentPeriod;
        }
        this->callback = cb;

        return errorCode::OK;
    }

    template <typename T>
    errorCode TckChannel<T>::start()
    {
        this->startCnt = getCycleCounter();
        this->triggered = true;
        return errorCode::OK;
    }

    template <typename T>
    errorCode TckChannel<T>::stop()
    {
        this->triggered = false;
        return errorCode::OK;
    }

    template <typename CounterType>
    errorCode TckChannel<CounterType>::triggerDirect(CounterType reload)
    {
        this->startCnt = getCycleCounter();
        this->nextPeriod = reload;
        this->currentPeriod = this->nextPeriod;
        this->triggered = true;
        return errorCode::OK;
    }

    template <typename T>
    errorCode TckChannel<T>::trigger(float delay) // µs
    {
        return triggerDirect(microsecondToCycles(delay));
    }

    template <typename counter_t>
    bool TckChannel<counter_t>::tick()
    {
        static bool lock = false;
        counter_t now = getCycleCounter();
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

    template <typename CounterType>
    CounterType TckChannel<CounterType>::microsecondToCycles(float microSecond) const
    {
        if (microSecond > getMaxMicros())
        {
            microSecond = getMaxMicros();
            postError(errorCode::periodOverflow);
        }
        return (CounterType)(microSecond * clock);
    }

    // SPECIALIZATIONS =================================================================================
    // 32bit Counter -------------------------------------------------------------------------

    template <>
    inline uint32_t TckChannel<uint32_t>::getCycleCounter()
    {
        return ARM_DWT_CYCCNT; //directly use the cycle counter for uint32_t
    }

    template <>
    inline float TckChannel<uint32_t>::getMaxMicros() const
    {
        return 0xF000'0000 / clock; // don't use full range otherwise tick might miss the turnover for large periods
    }

    // 64bit Counter -------------------------------------------------------------------------

    template <>
    inline uint64_t TckChannel<uint64_t>::getCycleCounter()
    {
        uint32_t now = ARM_DWT_CYCCNT; // (extend the cycle counter to 64 bit)
        if (now < lastCyCnt)
        {
            curHIGH++;
        }
        lastCyCnt = now;
        return (((uint64_t)curHIGH << 32) | now);
    }

    template <>
    inline float TckChannel<uint64_t>::getMaxMicros() const
    {
        return 0xFFFF'FFFF; // currently limited to 2^32 µs (71.6h). Could be extended to 2^64 but would require change of interface
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

#else // TeensyLC

    // Quick hack only, needs to be improved

    template <typename uint32_64_t>
    class TckChannel : public TckChannelBase
    {
     public:
        TckChannel() { triggered = false; }
        virtual ~TckChannel(){};

        errorCode begin(callback_t cb, float period, bool periodic) override
        {
            triggered = false;
            this->periodic = periodic;
            this->period = (uint32_t)period;
            this->callback = cb;

            return errorCode::OK;
        }

        errorCode start()
        {
            this->startCNT = micros();
            this->triggered = true;
            return errorCode::OK;
        }

        errorCode stop()
        {
            this->triggered = false;
            return errorCode::OK;
        }

        inline errorCode setPeriod(uint32_t microSeconds) override;
        inline uint32_t getPeriod(void);

        inline errorCode trigger(float delay) // µs
        {
            this->startCNT = micros();
            this->period = (uint32_t)delay;
            this->triggered = true;
            return errorCode::OK;
        }

        float getMaxPeriod() const override { return getMaxMicros() / 1E6; } // seconds

     protected:
        static constexpr float clock = F_CPU / 1'000'000.0f;
        float getMaxMicros() const { return 0xF000'0000 / clock; } // don't use full range otherwise tick might miss the turnover for large periods

        uint32_t startCNT, period;
        callback_t callback;
        bool triggered;
        bool periodic;

        inline bool tick();
        bool block = false;

        friend TCK_t;
    };

    // IMPLEMENTATION ==============================================

    template <typename ct>
    bool TckChannel<ct>::tick()
    {
        static bool lock = false;

        if (!lock && period != 0 && triggered && (micros() - startCNT) >= period)
        {
            lock = true;
            startCNT = micros();
            triggered = periodic; // i.e., stays triggerd if periodic, stops if oneShot
            callback();
            lock = false;
            return true;
        }
        return false;
    }

    template <typename ct>
    errorCode TckChannel<ct>::setPeriod(uint32_t microSeconds)
    {
        period = microSeconds;
        return errorCode::OK;
    }

    template <typename ct>
    uint32_t TckChannel<ct>::getPeriod()
    {
        return period;
    }

#endif

} // namespace TeensyTimerTool
