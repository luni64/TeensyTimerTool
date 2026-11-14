#pragma once

#include "ErrorHandling/error_codes.h"
#include "baseTimer.h"
#include "type_traits"

namespace TeensyTimerTool
{
    class OneShotTimer : public BaseTimer
    {
     public:
        inline OneShotTimer(TimerGenerator *generator = nullptr);

        inline errorCode begin(callback_t cb);
        inline errorCode start(); // not a one-shot operation (use trigger()): notImplemented
        // stop() (from BaseTimer) cancels a pending trigger
        template <typename T> errorCode trigger(T delay);
        template <typename T> errorCode triggerDirect(T reload);
        template <typename T> errorCode getTriggerReload(float delay, T *reload);
    };

    // Implementation ================================================

    OneShotTimer::OneShotTimer(TimerGenerator *generator)
        : BaseTimer(generator, false)
    {}

    errorCode OneShotTimer::begin(callback_t callback)
    {
        return BaseTimer::begin(callback, 0, false);
    }

    errorCode OneShotTimer::start()
    {
        return postError(errorCode::notSupported);
    }

    template <typename period_t>
    errorCode OneShotTimer::trigger(period_t delay)
    {
        if (timerChannel == nullptr) return postError(errorCode::notInitialized);
        float us = period2us(delay);
        if (us < 0) return postError(errorCode::invalidArgument);
        return postError(timerChannel->trigger(us));
    }

    template <typename T>
    errorCode OneShotTimer::triggerDirect(T reload)
    {
        if (timerChannel)
            return postError(timerChannel->triggerDirect(reload));

        return postError(errorCode::notInitialized);
    }

    template <typename T>
    errorCode OneShotTimer::getTriggerReload(float delay, T *reload)
    {
        if (timerChannel)
            return postError(timerChannel->getTriggerReload(delay, reload));

        return postError(errorCode::notInitialized);
    }
} // namespace TeensyTimerTool