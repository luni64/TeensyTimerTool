#pragma once

//#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "ITimerChannel.h"
#include <type_traits>

#if defined(USE_TIME_LITERALS)
#    include "frequency.h"
#    include <chrono>
using namespace std::chrono_literals;
using namespace std::chrono;
#endif

namespace TeensyTimerTool
{
    class BaseTimer
    {
     public:
        template <typename T>
        inline errorCode begin(callback_t callback, T period, bool start = true);
        inline errorCode setPrescaler(int psc);
        inline errorCode end();
        inline errorCode start();
        inline errorCode stop();

        inline float getMaxPeriod() const;

     protected:
       
        template <class T, std::enable_if_t<std::is_arithmetic<T>::value, int>* = nullptr>
        T getPeriod(T v) { return v; }


        BaseTimer(TimerGenerator* generator, bool periodic);

        TimerGenerator* timerGenerator;
        ITimerChannel* timerChannel;
        bool isPeriodic;
        uint32_t prescaler = 0;

#if defined(USE_TIME_LITERALS)
     public:
        template <typename dur = seconds>
        inline float getMaxDuration() const { return getMaxPeriod() * dur::period::den / dur::period::num; }

     protected:
        template <class T, std::enable_if_t<std::chrono::__is_duration<T>::value, int>* = nullptr>
        float getPeriod(T v) { return (duration_cast<duration<float, std::micro>>(v).count()); }

        template <class T, std::enable_if_t<TeensyTimerTool::__is_frequency<T>::value, int>* = nullptr>
        float getPeriod(T v) { return 1'000'000 / duration_cast<hertz>(v).count(); }
#endif
    };

    // INLINE IMPLEMENTATION ================================================

    template <typename T>
    errorCode BaseTimer::begin(callback_t callback, T p, bool start)
    {
        auto period = getPeriod(p);

        if (callback == nullptr) return postError(errorCode::callback);
        if (isPeriodic && period == 0) return postError(errorCode::reload);

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
        }

        errorCode result = timerChannel->begin(callback, period, isPeriodic);

        if (result == errorCode::OK)
        {
            if (isPeriodic && start) timerChannel->start();
        }
        return postError(result);
    }

    errorCode BaseTimer::end()
    {
        return postError(errorCode::notImplemented);
    }

    errorCode BaseTimer::start()
    {
        timerChannel->start();
        return errorCode::OK; // hack, implement return value in timer interface
    }

    errorCode BaseTimer::stop()
    {
        return postError(timerChannel->stop());
    }

    float BaseTimer::getMaxPeriod() const
    {
        if (timerChannel != nullptr)
        {
            return timerChannel->getMaxPeriod();
        }
        postError(errorCode::notInitialized);
        return 0;
    }
}