#pragma once

#include "baseTimer.h"

namespace TeensyTimerTool
{
    class PeriodicTimer : public BaseTimer
    {
     public:
        PeriodicTimer(TimerGenerator* generator = nullptr)
            : BaseTimer(generator, true) {}

        template <class T, std::enable_if_t<std::is_arithmetic<T>::value, int>* = nullptr>
        errorCode setPeriod(T p) { return postError(timerChannel->setPeriod((float)p)); }

        template <class T, std::enable_if_t<std::is_arithmetic<T>::value, int>* = nullptr>
        errorCode setNextPeriod(T p) { return postError(timerChannel->setNextPeriod((float)p)); }
    };

    // IMPLEMENTATION =====================================================================

}