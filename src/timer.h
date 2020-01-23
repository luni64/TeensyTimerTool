#pragma once

#include "types.h"
#include "ErrorHandling/ErrorCodes.h"
#include "ITimerChannel.h"

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

        inline void start() { timerChannel->start(); }
        inline void stop() { timerChannel->stop(); }

        static void attachErrFunc(errorFunc_t);

        ITimerChannel* timerChannel;

     protected:
         errorCode begin(callback_t cb, uint32_t period, bool periodic);
         errorCode postError(errorCode);

         TimerGenerator *timerGenerator;
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
