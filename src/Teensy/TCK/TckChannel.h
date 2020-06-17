#pragma once

#include "../../ITimerChannel.h"
#include "ErrorHandling/error_codes.h"
#include "core_pins.h"

namespace TeensyTimerTool
{
    class TCK_t;

#if defined(ARDUINO_TEENSYLC) // quick hack for T-LC, should be improved later (using systick?)

    class TckChannel : public ITimerChannel
    {
     public:
        inline TckChannel() { triggered = false; }
        inline virtual ~TckChannel(){};

        inline errorCode begin(callback_t cb, uint32_t period, bool periodic)
        {
            triggered = false;
            this->periodic = periodic;
            this->period = period;
            this->callback = cb;

            startCNT = micros();

            return errorCode::OK;
        }

        inline void start()
        {
            this->startCNT = micros();
            this->triggered = true;
        }

        inline errorCode stop()
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

     protected:
        uint32_t startCNT, period;
        callback_t callback;
        bool triggered;
        bool periodic;

        inline void tick();
        bool block = false;

        friend TCK_t;
    };

    // IMPLEMENTATION ==============================================

    void TckChannel::tick()
    {
        static bool lock = false;

        if (!lock && period != 0 && triggered && (micros() - startCNT) >= period)
        {
            lock = true;
            startCNT = micros();
            triggered = periodic; // i.e., stays triggerd if periodic, stops if oneShot
            callback();
            lock = false;
        }
    }
    errorCode TckChannel::setPeriod(uint32_t microSeconds)
    {
        period = microSeconds;
        return errorCode::OK;
    }
    uint32_t TckChannel::getPeriod()
    {
        return period;
    }

#else

    class TckChannel : public ITimerChannel
    {
     public:
        inline TckChannel() { triggered = false; }
        inline virtual ~TckChannel(){};

        errorCode begin(callback_t cb, uint32_t period, bool periodic)
        {
            this->triggered = false;
            this->periodic = periodic;
            this->currentPeriod = microsecondToCPUCycles(period);

            this->nextPeriod = this->currentPeriod;
            this->callback = cb;

            this->startCNT = ARM_DWT_CYCCNT;

            return errorCode::OK;
        }

        void start()
        {
            this->startCNT = ARM_DWT_CYCCNT;
            this->triggered = true;
        }

        errorCode stop()
        {
            this->triggered = false;
            return errorCode::OK;
        }

        inline errorCode setPeriod(uint32_t microSeconds) override;
        // inline uint32_t getPeriod(void);

        inline errorCode trigger(uint32_t delay) // µs
        {
            this->startCNT = ARM_DWT_CYCCNT;
            this->nextPeriod = microsecondToCPUCycles(delay);
            this->currentPeriod = this->nextPeriod - 68; //??? compensating for cycles spent computing?

            this->triggered = true;

            return errorCode::OK;
        }

        inline float getMaxPeriod()
        {
            return CPUCyclesToMicroseond(0xFFFF'FFFF);
        }

     protected:
        uint32_t startCNT, currentPeriod, nextPeriod;
        callback_t callback;
        bool triggered;
        bool periodic;

        inline void tick();
        bool block = false;

        friend TCK_t;

        static inline uint32_t microsecondToCPUCycles(uint32_t microSecond);

        static inline uint32_t CPUCyclesToMicroseond(uint32_t cpuCycles);
    };

    // IMPLEMENTATION ==============================================


    uint32_t TckChannel::microsecondToCPUCycles(uint32_t microSecond)
    {
        return microSecond * (F_CPU / 1'000'000);
    }

    uint32_t TckChannel::CPUCyclesToMicroseond(uint32_t cpuCycles)
    {
        return (1'000'000.0f / F_CPU) * cpuCycles;
    }

    void TckChannel::tick()
    {
        static bool lock = false;

        if (!lock && this->currentPeriod != 0 && this->triggered && (ARM_DWT_CYCCNT - this->startCNT) >= this->currentPeriod)
        {
            lock = true;
            this->startCNT = ARM_DWT_CYCCNT;
            this->triggered = this->periodic; // i.e., stays triggerd if periodic, stops if oneShot
            callback();
            lock = false;
        }
    }

    errorCode TckChannel::setPeriod(uint32_t microSeconds)
    {
        const uint32_t period = microsecondToCPUCycles(microSeconds);

        this->nextPeriod = period;
        this->currentPeriod = period;
        return errorCode::OK;
    }
    
    // uint32_t TckChannel::getPeriod()
    // {
    //     return period * (1'000'000.0f / F_CPU);
    // }

#endif

} // namespace TeensyTimerTool
