#pragma once

#include "../ErrorHandling/error_codes.h"
#include "IOneShotChannelEx.h"

namespace TeensyTimerTool
{
    class OneShotTimer
    {
     public:
        OneShotTimer(TimerModule module = TimerModule::usePool);
        ~OneShotTimer();

        errorCode begin(callback_t cb);

        template <typename delay_t> errorCode trigger(delay_t delay) const;
        template <typename reload_t> errorCode triggerDirect(reload_t reload) const;
        template <typename delay_t, typename reload_t> errorCode getTriggerReload(delay_t delay, reload_t *reload) const;

     protected:
        TimerModule timerModule;
        IOneShotChannelEx *timer;
    };

    // Inline Implementation ================================================

    template <typename delay_t>
    errorCode OneShotTimer::trigger(delay_t delay) const
    {
        float us = toMicroseconds(delay);
        return postError(timer ? timer->trigger(us) : errorCode::notInitialized);
    }

    template <typename period_t>
    errorCode OneShotTimer::triggerDirect(period_t reload) const
    {
        return postError(timer ? timer->triggerDirect(reload) : errorCode::notInitialized);
    }

    template <typename delay_t, typename reload_t>
    errorCode OneShotTimer::getTriggerReload(delay_t d, reload_t *reload) const
    {
        float delay = toMicroseconds(d);
        return postError(timer? timer->getTriggerReload(delay, reload) : errorCode::notInitialized);
    }
} // namespace TeensyTimerTool