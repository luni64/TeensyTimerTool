#pragma once

#include "../ITimerChannel.h"
#include "../types.h"
#include "Arduino.h"

namespace TeensyTimerTool
{
    class TCK_t;

    class TckChannel : public ITimerChannel
    {
     public:
        inline TckChannel() { triggered = false; }
        inline virtual ~TckChannel(){};

        inline void begin(callback_t cb, unsigned period, bool periodic)
        {
            Serial.println("begin");

            triggered = false;
            this->periodic = periodic;
            this->period = period * (F_CPU / 1'000'000);
            this->callback = cb;

            startCNT = ARM_DWT_CYCCNT;            
        }

        inline void start()
        {
            Serial.println("start");

            this->startCNT = ARM_DWT_CYCCNT;
            this->triggered = true;
        }

        inline void stop()
        {
            this->triggered = false;
        }

        // inline void setPeriod(uint32_t microSeconds);
        inline uint32_t getPeriod(void);

        inline void trigger(uint32_t delay) // µs
        {
            this->startCNT = ARM_DWT_CYCCNT;
            this->period = delay * (F_CPU / 1'000'000) - 68;
            this->triggered = true;
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

    // void TckChannel::setPeriod(uint32_t microSeconds)
    // {
    //     period = microSeconds * (F_CPU / 1'000'000) ;
    // }
    uint32_t TckChannel::getPeriod()
    {
        return period * (1'000'000.0f / F_CPU);
    }

} // namespace TeensyTimerTool
