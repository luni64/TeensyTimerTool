#pragma once

#include "baseTimer.h"

namespace TeensyTimerTool
{
    // How setPeriod() treats the currently running period
    enum class PeriodMode {
        restart,     // default: the new period starts now, the time elapsed so far is discarded
        keepElapsed, // the time elapsed so far counts; fires immediately if it already exceeds the new period
                     // (behaviour of TMR and TCK timers before v1.5, supported by TMR, TCK, TCK64, TCK_RTC)
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
            return postError(mode == PeriodMode::restart ? timerChannel->setPeriod(us) : timerChannel->setPeriodKeepElapsed(us));
        }

        template <typename period_t>
        inline errorCode setNextPeriod(period_t p)
        {
            if (timerChannel == nullptr) return postError(errorCode::notInitialized);
            return postError(timerChannel->setNextPeriod(period2us(p)));
        }
    };

    // IMPLEMENTATION =====================================================================

} // namespace TeensyTimerTool
