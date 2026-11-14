#pragma once

#include "baseTimer.h"

namespace TeensyTimerTool
{
    // How setPeriod() treats the currently running period
    enum class PeriodMode {
        restart,     // default: the new period starts now, the time elapsed so far is discarded
        keepElapsed, // the time elapsed so far counts; fires immediately if it already exceeds the new period
                     // (behaviour of TMR and TCK timers before v1.5; supported by PIT, GPT, TMR, TCK, TCK64, TCK_RTC)
    };

    class PeriodicTimer : public BaseTimer
    {
     public:
        PeriodicTimer(TimerGenerator *generator = nullptr) : BaseTimer(generator, true) {}

        template <typename period_t>
        inline errorCode setPeriod(period_t p, PeriodMode mode = PeriodMode::restart)
        {
            if (timerChannel == nullptr) return postError(errorCode::notInitialized);
            float us = period2us(p);
            if (us < 0) return postError(errorCode::invalidArgument);
            if (us == 0) return postError(errorCode::zeroPeriod);
            return postError(mode == PeriodMode::restart ? timerChannel->setPeriod(us) : timerChannel->setPeriodKeepElapsed(us));
        }

        // continues the period interrupted by stop(), as if the timer had not been stopped (the time while stopped
        // doesn't count). Without an interrupted period (never started, or start() / begin() since) like start();
        // no effect on a running timer. start() instead restarts the period.
        inline errorCode resume()
        {
            if (timerChannel == nullptr) return postError(errorCode::notInitialized);
            return postError(timerChannel->resume());
        }

        // the period set by the latest begin() / setPeriod() / setNextPeriod(), rounded to timer ticks;
        // seconds as float, or a std::chrono::duration (see BaseTimer::getMaxPeriod)
        template <typename T = float>
        T getPeriod() const { return query<T>(&ITimerChannel::getPeriod); }

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

    // IMPLEMENTATION =====================================================================

} // namespace TeensyTimerTool
