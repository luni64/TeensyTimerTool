#pragma once

#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "ITimerChannel.h"
#include "helpers.h"
#include <type_traits>

#if defined(USE_TIME_LITERALS)
//#include "frequency.h"
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
        template <typename period_t>
        inline errorCode begin(callback_t callback, period_t period, bool start = true);
        errorCode setPrescaler(int psc); // TMR (PSC_1..PSC_128) and GPT (PSC_1..PSC_4096), others: notImplemented; applied at begin() if called before
        inline errorCode end();
        inline errorCode start();
        inline errorCode stop();

        // Getters return seconds as float by default, or any std::chrono::duration, e.g.
        // getMaxPeriod<std::chrono::microseconds>() (integer durations are rounded to the nearest unit).
        // On errors: NAN for float/double, zero() for durations; the error is posted.
        template <typename T = float> T getMaxPeriod() const;
        template <typename T = float> T getRemainingTime() const; // time to the next callback, 0 if due / none pending
        template <typename T = float> T getResolution() const;    // one timer tick (with the current prescaler)

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

        if (callback == nullptr) return postError(errorCode::callback);
        if (period < 0) return postError(errorCode::argument);
        if (isPeriodic && period == 0) return postError(errorCode::reload);

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
            if (timerChannel == nullptr) return postError(errorCode::noFreeModule);

            if (hasPrescaler) // prescaler requested before a channel was available
            {
                pscResult = timerChannel->setPrescaler(prescaler);
                if (pscResult == errorCode::notImplemented) pscResult = errorCode::prescalerIgnored; // warning, timer runs
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
        if (std::isnan(s)) postError(errorCode::notImplemented);
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
        if (std::isnan(s)) postError(errorCode::notImplemented);
        return fromSeconds<T>(s);
    }

    template <typename T>
    T BaseTimer::getResolution() const
    {
        return query<T>(&ITimerChannel::getResolution);
    }
} // namespace TeensyTimerTool