#include "periodicTimer.h"
//#include "Arduino.h"

namespace TeensyTimerTool
{
    PeriodicTimer::PeriodicTimer(TimerModule _module)
        : module(_module), timer(nullptr)
    {}

    PeriodicTimer::~PeriodicTimer()
    {
        end();
    }

    float PeriodicTimer::getMaxPeriod() const
    {
        if (timer) return timer->getMaxPeriod();

        postError(errorCode::notInitialized);
        return 0;
    }

    errorCode PeriodicTimer::stop()
    {
        return timer ? timer->stop() : errorCode::notInitialized;
    }

    errorCode PeriodicTimer::start()
    {
        return postError(timer ? timer->start() : errorCode::notInitialized);
    }

    errorCode PeriodicTimer::end()
    {
        delete timer;
        timer = nullptr;
        return errorCode::OK;
    }
} // namespace TeensyTimerTool
