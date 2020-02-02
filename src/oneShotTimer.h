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
        inline errorCode stop();
    };


    // Implementation ================================================

    OneShotTimer::OneShotTimer(TimerGenerator* generator)
        : BaseTimer(generator, false)
    {}

    errorCode OneShotTimer::begin(callback_t callback)
    {
        return BaseTimer::begin(callback, 0,  false);
    }

    template <typename T>
    errorCode OneShotTimer::trigger(T delay)
    {
        static_assert(std::is_integral<T>() || std::is_floating_point<T>(), "Only floating point or integral types allowed");

        errorCode result;

        if (std::is_floating_point<T>())
            result = timerChannel->trigger((float) delay);
        else
            result = timerChannel->trigger((uint32_t) delay);

        return result;
    }

    errorCode OneShotTimer::stop()
    {
        return postError(errorCode::notImplemented);
    }
}