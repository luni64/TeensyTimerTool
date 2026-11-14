#pragma once

#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "ITimerChannel.h"
#include "helpers.h"
#include <type_traits>

#if defined(USE_TIME_LITERALS)
#include <chrono>
#include <cmath>
using namespace std::chrono_literals;
using namespace std::chrono;
#endif

namespace TeensyTimerTool
{
    class BaseTimer
    {
     public:
        /// @brief PeriodicTimer: allocates a timer channel (first call), sets the callback and the period and starts the timer.
        /// @details Called on a running timer, it restarts it with the new callback and period.
        /// @param callback function to invoke: plain function, lambda expression or functor, e.g. `[] { digitalToggleFast(13); }`
        /// @param period period in µs (e.g. 500, 12.5) or as std::chrono duration (e.g. 250ms, 1.5s)
        /// @param start true (default): start now; false: start later with start()
        /// @return OK; periodOverflow (warning, max. period used); invalidArgument; noCallback; zeroPeriod;
        ///         noFreeChannel (timer type in use); poolExhausted (no free timer in the pool)
        template <typename period_t>
        inline errorCode begin(callback_t callback, period_t period, bool start = true);
        /// @brief Sets the prescaler, which divides the timer clock: longer max. period, coarser resolution.
        /// @details Before begin(): stored and applied by begin(). On a running timer: takes effect immediately,
        /// the period keeps its number of ticks until the next setPeriod().
        /// @param psc PSC_1, PSC_2 … PSC_128 (TMR), … PSC_4096 (GPT)
        /// @return OK; invalidArgument (value not valid for this timer); notSupported (timer without prescaler)
        errorCode setPrescaler(int psc);

        /// @brief Stops the timer and releases its channel for other timers; a later begin() allocates a new one.
        /// @return OK (always)
        inline errorCode end();

        /// @brief Starts a stopped periodic timer (or one set up with begin(…, false)); the period starts from the beginning.
        /// @details To continue the interrupted period instead, use PeriodicTimer::resume().
        /// OneShotTimer: use trigger() instead.
        /// @return OK; notInitialized (before begin()); OneShotTimer: notSupported
        inline errorCode start();

        /// @brief Stops the timer: no further callbacks. On a one-shot timer, a pending trigger is cancelled.
        /// @details The timer keeps its channel and settings; start(), resume() or trigger() use it again.
        /// @return OK (also if not running); notInitialized (before begin())
        inline errorCode stop();

        /// @brief Returns the longest period or delay the timer supports with its current prescaler.
        /// @tparam T float (default, seconds), double, or a std::chrono duration, e.g. getMaxPeriod<std::chrono::milliseconds>()
        /// @return the max. period; NAN / zero() if called before begin()
        template <typename T = float> T getMaxPeriod() const;

        /// @brief Returns the time until the next callback; 0 if none is pending (stopped, one-shot fired).
        /// @tparam T float (default, seconds), double, or a std::chrono duration
        /// @return the remaining time; NAN / zero() before begin() or if not supported (RTC, FTM)
        template <typename T = float> T getRemainingTime() const;

        /// @brief Returns the duration of one timer tick with the current prescaler, e.g. 41.7 ns for the PIT.
        /// @tparam T float (default, seconds), double, or a std::chrono duration, e.g. getResolution<std::chrono::duration<double, std::nano>>()
        /// @return the tick duration; NAN / zero() before begin() or if not supported (FTM)
        template <typename T = float> T getResolution() const;

        // A timer owns its channel: no copies (the channel would be released twice), but timers can be
        // moved (e.g. stored in a std::vector). The moved-from timer is empty, begin() allocates a new channel.
        BaseTimer(const BaseTimer &)            = delete;
        BaseTimer &operator=(const BaseTimer &) = delete;

     protected:
        template <typename T> static T fromSeconds(double s);
        template <typename T> T query(double (ITimerChannel::*getter)() const) const;

        BaseTimer(TimerGenerator *generator, bool periodic);
        BaseTimer(BaseTimer &&other) noexcept;
        BaseTimer &operator=(BaseTimer &&other) noexcept;
        virtual ~BaseTimer();

        TimerGenerator *timerGenerator;
        ITimerChannel *timerChannel;
        bool isPeriodic;
        int prescaler     = 0;
        bool hasPrescaler = false; // true if setPrescaler() was called (PSC_AUTO is -1, can't be a marker)
    };

    // INLINE IMPLEMENTATION ================================================

    template <typename period_t>
    errorCode BaseTimer::begin(callback_t callback, period_t p, bool start)
    {
        float period = period2us(p); // transform from any period type to microseconds (float)

        if (callback == nullptr) return postError(errorCode::noCallback);
        if (period < 0) return postError(errorCode::invalidArgument);
        if (isPeriodic && period == 0) return postError(errorCode::zeroPeriod);

        errorCode pscResult = errorCode::OK;

        if (timerChannel == nullptr)
        {
            if (timerGenerator != nullptr) // use timer passed in during construction
            {
                timerChannel = timerGenerator();
                if (timerChannel == nullptr) return postError(errorCode::noFreeChannel);
            } else //find the next free timer
            {
                for (unsigned i = 0; timerChannel == nullptr && i < timerCnt; i++)
                {
                    timerChannel = timerPool[i]();
                }
            }
            if (timerChannel == nullptr) return postError(errorCode::poolExhausted);

            if (hasPrescaler) // prescaler requested before a channel was available
            {
                pscResult = timerChannel->setPrescaler(prescaler);
                if (pscResult == errorCode::notSupported) pscResult = errorCode::prescalerIgnored; // warning, timer runs
            }
        }

        errorCode result = timerChannel->begin(callback, period, isPeriodic);

        if ((int)result <= 0) // OK or a warning (e.g. periodOverflow: runs with the max. period)
        {
            if (isPeriodic && start) timerChannel->start();
            if (result == errorCode::OK) result = pscResult; // timer runs, but report a failed setPrescaler() (e.g. notImplemented)
        }
        return postError(result);
    }

    errorCode BaseTimer::end() // stop the timer and release its channel; begin() allocates a new one
    {
        if (timerChannel != nullptr)
        {
            timerChannel->release();
            timerChannel = nullptr;
        }
        return errorCode::OK; // idempotent, end() on a not running timer is fine
    }

    errorCode BaseTimer::start()
    {
        if (timerChannel)
            return postError(timerChannel->start());

        return postError(errorCode::notInitialized);
    }

    errorCode BaseTimer::stop()
    {
        if (timerChannel)
            return postError(timerChannel->stop());

        return postError(errorCode::notInitialized);
    }

    template <typename T>
    T BaseTimer::fromSeconds(double s)
    {
        if constexpr (std::is_arithmetic_v<T>)
            return (T)s;
#if defined(USE_TIME_LITERALS)
        else
        {
            static_assert(std::chrono::__is_duration<T>::value, "getters return float, double or a std::chrono::duration");
            using namespace std::chrono;
            if (std::isnan(s)) return T::zero();
            duration<double> d(s);
            if (d >= duration_cast<duration<double>>(T::max())) return T::max(); // e.g. TCK64 max. period in ns
            if constexpr (std::is_floating_point_v<typename T::rep>)
                return duration_cast<T>(d);
            else
                return std::chrono::round<T>(d);
        }
#else
        else
            static_assert(std::is_arithmetic_v<T>, "std::chrono getters need USE_TIME_LITERALS");
#endif
    }

    // calls a channel getter (seconds, NAN = not implemented), posts errors, converts to T
    template <typename T>
    T BaseTimer::query(double (ITimerChannel::*getter)() const) const
    {
        if (timerChannel == nullptr)
        {
            postError(errorCode::notInitialized);
            return fromSeconds<T>(NAN);
        }
        double s = (timerChannel->*getter)();
        if (std::isnan(s)) postError(errorCode::notSupported);
        return fromSeconds<T>(s);
    }

    template <typename T>
    T BaseTimer::getMaxPeriod() const
    {
        if (timerChannel == nullptr)
        {
            postError(errorCode::notInitialized);
            return fromSeconds<T>(NAN);
        }
        return fromSeconds<T>(timerChannel->getMaxPeriod());
    }

    template <typename T>
    T BaseTimer::getRemainingTime() const
    {
        if (timerChannel == nullptr)
        {
            postError(errorCode::notInitialized);
            return fromSeconds<T>(NAN);
        }
        double s = timerChannel->getRemainingTime();
        if (std::isnan(s)) postError(errorCode::notSupported);
        return fromSeconds<T>(s);
    }

    template <typename T>
    T BaseTimer::getResolution() const
    {
        return query<T>(&ITimerChannel::getResolution);
    }
} // namespace TeensyTimerTool