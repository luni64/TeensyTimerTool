#include "baseTimer.h"
#include "types.h"

namespace TeensyTimerTool
{
    BaseTimer::BaseTimer(TimerGenerator* generator, bool periodic)
        : timerGenerator(generator)
    {
        this->timerGenerator = generator;
        this->timerChannel = nullptr;
        this->isPeriodic = periodic;
    }

    errorCode BaseTimer::postError(errorCode e)
    {
        if (errFunc != nullptr) errFunc(e);
        return e;
    }

    errorFunc_t BaseTimer::errFunc = nullptr;
}