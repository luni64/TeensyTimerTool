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

    template <typename period_t>
    errorCode OneShotTimer::trigger(period_t delay)
    {
        return timerChannel->trigger(period2us(delay));
    }

    template <typename T>
    errorCode OneShotTimer::triggerDirect(T reload)
    {
        return timerChannel->triggerDirect(reload);
    }

    template <typename T>
    errorCode OneShotTimer::getTriggerReload(float delay, T *reload)
    {
        return timerChannel->getTriggerReload(delay, reload);
    }
} // namespace TeensyTimerTool