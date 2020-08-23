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
        TckChannel() { triggered = false; }
        virtual ~TckChannel(){}; //TBD

        inline errorCode begin(callback_t cb, uint32_t period, bool periodic) override;
        inline errorCode begin(callback_t cb, float period, bool periodic) override;
        inline errorCode start() override;
        inline errorCode stop() override;
        inline errorCode trigger(uint32_t delay_us) override;

        float getMaxPeriod() override  { return getMaxMicros() / 1E6; }  // seconds

        CounterType getCycleCounter() { postError(errorCode::wrongType); }

        // inline errorCode setPeriod(uint32_t microSeconds) override;
        // inline errorCode setCurrentPeriod(uint32_t microSeconds) override;
        // inline errorCode setNextPeriod(uint32_t microSeconds) override;

     protected:
        inline bool tick();
        inline CounterType microsecondToCPUCycles(const float microSecond) const;
        uint32_t getMaxMicros() const;

        callback_t callback;

        bool triggered;
        bool periodic;

        bool block = false;


        CounterType startCnt, currentPeriod, nextPeriod;

        uint32_t lastCyCnt;
        uint32_t curHIGH;

        friend TCK_t;

        // inline uint32_t CPUCyclesToMicroseond(const uint32_t cpuCycles);
        // inline errorCode _setCurrentPeriod(const uint32_t period);
        // inline void _setNextPeriod(const uint32_t period);
    };

    // IMPLEMENTATION ==============================================

    template <typename T>
    errorCode TckChannel<T>::begin(callback_t cb, uint32_t period, bool periodic)
    {
        this->triggered = false;
        this->periodic = periodic;
        this->currentPeriod = microsecondToCPUCycles(period);

        this->nextPeriod = this->currentPeriod;
        this->callback = cb;

        this->startCnt = getCurCycleCounter();

        return errorCode::OK;
    }

    template <typename T>
    errorCode TckChannel<T>::start()
    {
        this->startCnt = getCurCycleCounter();
        this->triggered = true;
        return errorCode::OK;
    }

    template <typename T>
    errorCode TckChannel<T>::stop()
    {
        this->triggered = false;
        return errorCode::OK;
    }

    template <typename T>
    errorCode TckChannel<T>::trigger(uint32_t delay) // µs
    {
        this->startCnt = getCurCycleCounter();
        this->nextPeriod = microsecondToCPUCycles(delay);
        this->currentPeriod = this->nextPeriod - 68; //??? compensating for cycles spent computing?

        this->triggered = true;

        return errorCode::OK;
    }

    template <typename counter_t>
    bool TckChannel<counter_t>::tick()
    {
        static bool lock = false;
        counter_t now = getCurCycleCounter();
        if (!lock && this->currentPeriod != 0 && this->triggered && (now - this->startCnt) >= this->currentPeriod)
        {
            lock = true;
            this->startCnt = now;
            this->triggered = this->periodic; // i.e., stays triggerd if periodic, stops if oneShot
            callback();
            lock = false;
            return true;
        } else
        {
            return false;
        }
    }

    template <typename ct>
    ct TckChannel<ct>::microsecondToCPUCycles(uint32_t microSecond)
    {
        if (microSecond > getMaxMicros())
        {
            microSecond = getMaxMicros();
            postError(errorCode::periodOverflow);
        }
        return microSecond * ct(F_CPU / 1'000'000);
    }

    // SPECIALIZATIONS =================================================================================
    // 32bit Counter -------------------------------------------------------------------------

    template <>
    inline uint32_t TckChannel<uint32_t>::getCurCycleCounter()
    {
        return ARM_DWT_CYCCNT; //directly use the cycle counter for uint32_t
    }

    template <>
    inline uint32_t TckChannel<uint32_t>::getMaxMicros() const
    {
        return (1E6 * 0xF000'0000) / F_CPU; // don't use full range otherwise tick might miss the turnover for large periods
    }

    // 64bit Counter -------------------------------------------------------------------------

    template <>
    inline uint64_t TckChannel<uint64_t>::getCurCycleCounter()
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
    inline uint32_t TckChannel<uint64_t>::getMaxMicros() const
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
    // errorCode TckChannel<ct>::setCurrentPeriod(uint32_t microSeconds)
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

#else

    // Quick hack only, needs to be improved

    template <typename uint32_64_t>
    class TckChannel : public TckChannelBase
    {
     public:
        TckChannel() { triggered = false; }
        virtual ~TckChannel(){};

        errorCode begin(callback_t cb, uint32_t period, bool periodic)
        {
            triggered = false;
            this->periodic = periodic;
            this->period = period;
            this->callback = cb;

            startCNT = micros();

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

        inline errorCode trigger(uint32_t delay) // µs
        {
            this->startCNT = micros();
            this->period = delay;
            this->triggered = true;
            return errorCode::OK;
        }

        float getMaxPeriod()
        {
            return (float)0xFFFF'FFFF / 1E6;
        }

     protected:
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
