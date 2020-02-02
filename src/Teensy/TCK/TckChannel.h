#pragma once

#include "../../ITimerChannel.h"
#include "ErrorHandling/error_codes.h"
#include "core_pins.h"

namespace TeensyTimerTool
{
    class TCK_t;

#if defined(TLC) // quick hack for T-LC, should be improved later (using systick?)

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

        inline void setPeriod(uint32_t microSeconds);
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
    void TckChannel::setPeriod(uint32_t microSeconds)
    {
        period = microSeconds;
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

        inline errorCode begin(callback_t cb, uint32_t period, bool periodic)
        {
            triggered = false;
            this->periodic = periodic;
            this->period = period * (F_CPU / 1'000'000);
            this->callback = cb;

            startCNT = ARM_DWT_CYCCNT;

            return errorCode::OK;
        }

        inline void start()
        {
            this->startCNT = ARM_DWT_CYCCNT;
            this->triggered = true;
        }

        inline errorCode stop()
        {
            this->triggered = false;
            return errorCode::OK;
        }

        inline void setPeriod(uint32_t microSeconds);
        inline uint32_t getPeriod(void);

        inline errorCode trigger(uint32_t delay) // µs
        {
            this->startCNT = ARM_DWT_CYCCNT;
            this->period = delay * (F_CPU / 1'000'000) - 68;
            this->triggered = true;

            return errorCode::OK;
        }

         inline float getMaxPeriod() override
         {
             return 1'000'000.0f / F_CPU * 0xFFFF'FFFF;
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

        if (!lock && period != 0 && triggered && (ARM_DWT_CYCCNT - startCNT) >= period)
        {
            lock = true;
            startCNT = ARM_DWT_CYCCNT;
            triggered = periodic; // i.e., stays triggerd if periodic, stops if oneShot
            callback();
            lock = false;
        }
    }

    void TckChannel::setPeriod(uint32_t microSeconds)
    {
        period = microSeconds * (F_CPU / 1'000'000);
    }
    uint32_t TckChannel::getPeriod()
    {
        return period * (1'000'000.0f / F_CPU);
    }

#endif

} // namespace TeensyTimerTool
