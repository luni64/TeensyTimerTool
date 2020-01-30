#pragma once

#include "ErrorHandling/error_codes.h"
#include "ITimerChannel.h"
#include "types.h"

namespace TeensyTimerTool
{
    class Timer
    {
     public:
        Timer(TimerGenerator* gen = nullptr);

        inline errorCode beginPeriodic(callback_t cb, uint32_t period) { return begin(cb, period, true); }
        inline errorCode beginOneShot(callback_t cb) { return begin(cb, 0, false); }
        inline void trigger(uint32_t delay);
        inline void setPeriod(uint32_t microSeconds);
        inline uint32_t getPeriod();

        void start() { timerChannel->start(); }
        void stop() { timerChannel->stop(); }        
        
        ITimerChannel* getChannel(){return timerChannel;}

        static void attachErrFunc(errorFunc_t);

     protected:
        ITimerChannel* timerChannel;
        errorCode begin(callback_t cb, uint32_t period, bool periodic);
        
        inline static errorCode postError(errorCode);

        TimerGenerator* timerGenerator;
        static errorFunc_t errFunc;
    };

    // IMPLEMENTATION =======================================================

    void Timer::trigger(const uint32_t delay)
    {
        timerChannel->trigger(delay);
    }

    void Timer::setPeriod(uint32_t microseconds)
    {
        timerChannel->setPeriod(microseconds);
    }

    uint32_t Timer::getPeriod()
    {
        return timerChannel->getPeriod();
    }
}
