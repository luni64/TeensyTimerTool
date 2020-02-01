#pragma once

#include "ErrorHandling/error_codes.h"
#include "ITimerChannel.h"
#include "types.h"


namespace TeensyTimerTool
{
    class BaseTimer
    {
     public:
        inline errorCode begin(callback_t callback, uint32_t period, bool start = true);
        inline errorCode end() { return errorCode::notImplemented; }
        inline errorCode stop() { return timerChannel->stop(); }

     protected:
        BaseTimer(TimerGenerator* generator, bool periodic);

        TimerGenerator* timerGenerator;
        ITimerChannel* timerChannel;
        bool isPeriodic;
    };



    // INLINE IMPLEMENTATION ====================================

    errorCode BaseTimer::begin(callback_t callback, uint32_t reload, bool start)
    {
        if (callback == nullptr) return postError(errorCode::callback);
        if (isPeriodic && reload == 0) return postError(errorCode::reload);

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

        timerChannel->begin(callback, reload, isPeriodic);
        if (isPeriodic && start) timerChannel->start();

        return errorCode::OK;
    }
}