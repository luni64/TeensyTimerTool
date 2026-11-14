#pragma once

#include "baseTimer.h"

namespace TeensyTimerTool
{
    /// @brief How setPeriod() treats the period that is currently running.
    enum class PeriodMode {
        restart,     ///< default: the new period starts at the call, the time elapsed so far is discarded
        keepElapsed, ///< the time elapsed so far counts; fires at once if it already exceeds the new period (PIT, TMR, TCK*)
    };

    /// @brief Timer that invokes its callback at a fixed interval.
    /// @details Declare it with a timer type, e.g. `PeriodicTimer t1(TCK);`, or without one to take the next
    /// free timer from the timer pool. Start it with begin(callback, period).
    /// Manual: https://luni64.github.io/TeensyTimerTool/reference/periodic-timer/
    class PeriodicTimer : public BaseTimer
    {
     public:
        /// @brief Creates the timer object; begin() allocates the hardware.
        /// @param generator timer type, e.g. PIT, TMR1, GPT1, TCK; nullptr (default): next free timer of the pool
        PeriodicTimer(TimerGenerator *generator = nullptr) : BaseTimer(generator, true) {}

        /// @brief Changes the period. A stopped timer stays stopped; start() or resume() use the new period.
        /// @param p new period in µs (e.g. 500, 12.5) or as std::chrono duration (e.g. 2ms, 1.5s)
        /// @param mode PeriodMode::restart (default): the new period starts now;
        ///             PeriodMode::keepElapsed: the time elapsed in the current period counts
        /// @return OK; periodOverflow (warning, max. period used); invalidArgument (negative); zeroPeriod;
        ///         notInitialized (before begin()); notSupported (keepElapsed on GPT / RTC, FTM)
        template <typename period_t>
        inline errorCode setPeriod(period_t p, PeriodMode mode = PeriodMode::restart)
        {
            if (timerChannel == nullptr) return postError(errorCode::notInitialized);
            float us = period2us(p);
            if (us < 0) return postError(errorCode::invalidArgument);
            if (us == 0) return postError(errorCode::zeroPeriod);
            return postError(mode == PeriodMode::restart ? timerChannel->setPeriod(us) : timerChannel->setPeriodKeepElapsed(us));
        }

        /// @brief Continues a timer stopped with stop() with the rest of the interrupted period.
        /// @details The time while stopped doesn't count. Without an interrupted period it works like start();
        /// on a running timer it has no effect. start() instead restarts the period.
        /// @return OK; notInitialized (before begin()); notSupported (RTC, FTM)
        inline errorCode resume()
        {
            if (timerChannel == nullptr) return postError(errorCode::notInitialized);
            return postError(timerChannel->resume());
        }

        /// @brief Returns the period set by the latest begin(), setPeriod() or setNextPeriod(), rounded to timer ticks.
        /// @tparam T float (default, seconds), double, or a std::chrono duration, e.g. getPeriod<std::chrono::microseconds>()
        /// @return the period; NAN / zero() if called before begin() or not supported (FTM)
        template <typename T = float>
        T getPeriod() const { return query<T>(&ITimerChannel::getPeriod); }

        /// @brief Sets the period that starts when the current period ends; the next callback occurs as scheduled.
        /// @details Called from the callback, it generates exact period sequences.
        /// @param p new period in µs or as std::chrono duration
        /// @return OK; periodOverflow (warning); invalidArgument; zeroPeriod; notInitialized;
        ///         notSupported (GPT, RTC, FTM)
        template <typename period_t>
        inline errorCode setNextPeriod(period_t p)
        {
            if (timerChannel == nullptr) return postError(errorCode::notInitialized);
            float us = period2us(p);
            if (us < 0) return postError(errorCode::invalidArgument);
            if (us == 0) return postError(errorCode::zeroPeriod);
            return postError(timerChannel->setNextPeriod(us));
        }
    };

} // namespace TeensyTimerTool
