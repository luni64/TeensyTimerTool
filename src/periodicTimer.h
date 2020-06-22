#pragma once

#include "baseTimer.h"

namespace TeensyTimerTool
{
    class PeriodicTimer : public BaseTimer
    {
     public:
        PeriodicTimer(TimerGenerator* generator = nullptr)
            : BaseTimer(generator, true) {}

        //inline errorCode start();
    };


    // IMPLEMENTATION =====================================================================


    // errorCode PeriodicTimer::start()
    // {
    //     return errorCode::notImplemented;
    // }

    // errorCode PeriodicTimer::stop()
    // {
    //     return errorCode::notImplemented;
    // }
}