#include "timer.h"
#include "config.h"


namespace TeensyTimerTool
{
    Timer::Timer(TimerGenerator* generator)
    :BaseTimer(generator, true)
    {
    }

    void Timer::attachErrFunc(errorFunc_t _errFunc)
    {
        errFunc = _errFunc;
    }
}
