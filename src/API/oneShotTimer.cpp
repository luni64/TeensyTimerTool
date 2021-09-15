#include "timerFactory.h"
#include "oneShotTimer.h"

namespace TeensyTimerTool
{
    OneShotTimer::OneShotTimer(TimerModule _module)
        : timerModule(_module), timer(nullptr)
    {}

    OneShotTimer::~OneShotTimer()
    {
        delete timer;
    }

    errorCode OneShotTimer::begin(callback_t callback)
    {
        if (!timer) // don't realloc if we already have a timer channel
        {
            if (timerModule != TimerModule::usePool) // use the specified module
            {
                timer = (IOneShotChannelEx *)TimerFactory::makeTimer(timerModule, TimerType::oneShot);
                if (!timer) return postError(errorCode::noFreeTimer);
            }
            else // use a module from the pool
            {
                return postError(errorCode::notImplemented);
            }
        }
        return postError(timer->begin(callback));
    }
}