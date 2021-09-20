#pragma once

#include "IPeriodicChannel.h"
#include "helpers.h"
#include "timerFactory.h"
#include "types.h"

namespace TeensyTimerTool
{
    class PeriodicTimer
    {
     public:
        PeriodicTimer(TimerModule module = TimerModule::usePool);
        ~PeriodicTimer();

        template <typename period_t>
        errorCode begin(callback_t callback, period_t period, bool start = true);
        errorCode start();
        errorCode stop();
        errorCode end();

        inline float getMaxPeriod() const;

        // template <class T, std::enable_if_t<std::is_arithmetic<T>::value, int>* = nullptr>
        // errorCode setPeriod(T p) { return postError(timerChannel->setPeriod((float)p)); }

        // template <class T, std::enable_if_t<std::is_arithmetic<T>::value, int>* = nullptr>
        // errorCode setNextPeriod(T p) { return postError(timerChannel->setNextPeriod((float)p)); }

     protected:
        TimerModule module;
        IPeriodicChannelEx *timer;
    };

    //Inline Implementatiton =============================================================

    template <typename period_t>
    errorCode PeriodicTimer::begin(callback_t callback, period_t p, bool start)
    {
        float period = toMicroseconds(p); // Transform arithmetic, duration and frequency types to microseconds
        if (callback == nullptr || period == 0) return postError(errorCode::arguments);

        if (!timer)
        {
            if (module != TimerModule::usePool)
            {
                timer = (IPeriodicChannelEx *)TimerFactory::makeTimer(module, TimerType::periodic);
                if (!timer) return postError(errorCode::noFreeTimer);
            } else
            {
                return postError(errorCode::notImplemented);
            }
        }
        return postError(timer->begin(callback, period, start));
    }
}