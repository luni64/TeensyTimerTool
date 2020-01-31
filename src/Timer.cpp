#include "timer.h"
#include "config.h"

namespace TeensyTimerTool
{
    Timer::Timer(TimerGenerator* generator)
    {
        this->timerGenerator = generator;
        this->timerChannel = nullptr;
    }

    errorCode Timer::begin(callback_t callback, uint32_t reload, bool periodic)
    {
        if (callback == nullptr) return postError(errorCode::callback);
        if (periodic && reload == 0) return postError(errorCode::reload);

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

        errorCode err = timerChannel->begin(callback, reload, periodic); 
        if(err != errorCode::OK) postError(err);


        if (periodic) timerChannel->start();

        return errorCode::OK;
    }

    void Timer::attachErrFunc(errorFunc_t _errFunc)
    {
        errFunc = _errFunc;
    }

    errorCode Timer::postError(errorCode e)
    {
        if (errFunc != nullptr) errFunc(e);
        return e;
    }

    errorFunc_t Timer::errFunc = nullptr;
}
