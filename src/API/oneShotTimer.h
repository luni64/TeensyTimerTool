#pragma once

#include "ErrorHandling/error_codes.h"
#include "baseTimer.h"
#include "type_traits"

namespace TeensyTimerTool
{
    /// @brief Timer that invokes its callback once, a given delay after trigger().
    /// @details Declare it with a timer type, e.g. `OneShotTimer t1(TMR1);`, or without one to take the next
    /// free timer from the timer pool. Set the callback with begin(), then call trigger(delay).
    /// Manual: https://luni64.github.io/TeensyTimerTool/reference/one-shot-timer/
    class OneShotTimer : public BaseTimer
    {
     public:
        /// @brief Creates the timer object; begin() allocates the hardware.
        /// @param generator timer type, e.g. PIT, TMR1, GPT1, TCK; nullptr (default): next free timer of the pool
        inline OneShotTimer(TimerGenerator *generator = nullptr);

        /// @brief Allocates a timer channel (first call) and sets the callback. The timer is not started.
        /// @param cb function to invoke: plain function, lambda expression or functor
        /// @return OK; prescalerIgnored (warning); invalidArgument (invalid setPrescaler() value before);
        ///         noCallback; noFreeChannel; poolExhausted; notSupported (RTC)
        inline errorCode begin(callback_t cb);

        /// @brief Not a one-shot operation, use trigger(). Always returns notSupported, the timer is unchanged.
        inline errorCode start();

        /// @brief Starts the timer: the callback is invoked once, `delay` after the call. Returns immediately.
        /// @details Called while a trigger is pending, it restarts the delay. Called from the callback, it
        /// re-triggers the timer (chained delays).
        /// @param delay delay in µs (e.g. 500, 12.5) or as std::chrono duration (e.g. 20ms); 0: after one tick
        /// @return OK; periodOverflow (warning, max. delay used); invalidArgument (negative); notInitialized (before begin())
        template <typename T> errorCode trigger(T delay);

        /// @brief Like trigger(), but with the delay in timer ticks from getTriggerReload(); saves the conversion.
        /// @param reload delay in ticks: uint32_t; uint64_t for TCK64 and TCK_RTC
        /// @return OK; periodOverflow (warning); notInitialized; notSupported (timer / reload type)
        template <typename T> errorCode triggerDirect(T reload);

        /// @brief Converts a delay into the reload value for triggerDirect(), rounded like trigger().
        /// @param delay delay in µs
        /// @param reload receives the reload value: uint32_t; uint64_t for TCK64 and TCK_RTC
        /// @return OK; periodOverflow (warning, *reload is the max.); notInitialized; notSupported
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
