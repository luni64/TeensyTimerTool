#pragma once

#include "baseTimer.h"

namespace TeensyTimerTool
{
    class PeriodicTimer : public BaseTimer
    {
     public:
        PeriodicTimer(TimerGenerator* generator = nullptr)
            : BaseTimer(generator, true) {}


    };


    // IMPLEMENTATION =====================================================================


}