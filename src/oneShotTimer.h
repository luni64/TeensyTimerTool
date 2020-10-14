#pragma once

#include "ErrorHandling/error_codes.h"
#include "baseTimer.h"
#include "type_traits"

namespace TeensyTimerTool
{
    class OneShotTimer : public BaseTimer
    {
     public:
        inline OneShotTimer(TimerGenerator* generator = nullptr);

        inline errorCode begin(callback_t cb);
        template <typename T> errorCode trigger(T delay);
        template <typename T> errorCode triggerDirect(T reload);
        template <typename T> errorCode getTriggerReload(float delay, T* reload);

        #if defined(USE_TIME_LITERALS)
        template <typename T, typename ratio>
        errorCode trigger(duration<T, ratio> _delay)
        {
            T delay = duration_cast<microseconds>(_delay).count();
            return trigger(delay);
        }
        #endif
    };

    // Implementation ================================================

    OneShotTimer::OneShotTimer(TimerGenerator* generator)
        : BaseTimer(generator, false)
    {}

    errorCode OneShotTimer::begin(callback_t callback)
    {
        return BaseTimer::begin(callback, 0, false);
    }

    template <typename T>
    errorCode OneShotTimer::trigger(T delay)
    {
        static_assert(std::is_integral<T>() || std::is_floating_point<T>(), "Only floating point or integral types allowed");

        errorCode result;

        if (std::is_floating_point<T>())
            result = timerChannel->trigger((float)delay);
        else
            result = timerChannel->trigger((uint32_t)delay);

        return result;
    }

    template <typename T>
    errorCode OneShotTimer::triggerDirect(T reload)
    {
        return timerChannel->triggerDirect(reload);
    }

    template <typename T>
    errorCode OneShotTimer::getTriggerReload(float delay, T* reload)
    {
        return timerChannel->getTriggerReload(delay, reload);
    }
}