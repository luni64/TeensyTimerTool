#pragma once

//#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "ITimerChannel.h"
#include <type_traits>

#if defined(USE_TIME_LITERALS)
  #include <chrono>
  #include "frequency.h"
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

        #if defined(USE_TIME_LITERALS)
        template <typename T, typename ratio>
        errorCode begin(callback_t callback, duration<T, ratio> period, bool start = true)
        {
            T p = duration_cast<microseconds>(period).count();
            return begin(callback, p, start);
        }

        template <typename T, typename ratio>
        errorCode begin(callback_t callback, frequency<T, ratio> f, bool start = true)
        {
            T freq = ((hertz)f).count();
            return begin(callback, 1E6f/freq, start);
        }
        #endif


#if defined(ENABLE_ADVANCED_FEATURES)
        ITimerChannel* getChannel()
        {
            return timerChannel;
        }
#endif

     protected:
        BaseTimer(TimerGenerator* generator, bool periodic);

        TimerGenerator* timerGenerator;
        ITimerChannel* timerChannel;
        bool isPeriodic;
        uint32_t prescaler = 0;
    };

    // INLINE IMPLEMENTATION ================================================

 template <typename T>
    errorCode BaseTimer::begin(callback_t callback, T period, bool start)
    {
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

        static_assert(std::is_floating_point<T>() || std::is_integral<T>(), "only floating point or integral types allowed");


        errorCode err = std::is_floating_point<T>() ? timerChannel->begin(callback, (float)period, isPeriodic) : timerChannel->begin(callback, (uint32_t)period, isPeriodic);

        if (err == errorCode::OK && isPeriodic && start)
        {
            timerChannel->start();
        }

        return err;
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
            return timerChannel->getMaxPeriod();

        postError(errorCode::notInitialized);
        return 0;
    }
}