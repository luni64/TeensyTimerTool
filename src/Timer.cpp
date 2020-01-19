#include "TeensyTimerTool.h"
//#include "timer.h"
//#include "config.h"

namespace TeensyTimerTool
{
    Timer::Timer(TimerGenerator* generator)
    {
        this->timerGenerator = generator;
        this->timerChannel = nullptr;
    }

    error Timer::begin(callback_t cb, uint32_t reload, bool oneShot)
    {

        if (cb == nullptr)
        {
            return postError(error::argument);
        }

        if (timerChannel == nullptr)
        {
            if (timerGenerator != nullptr) // use timer passed in during construction
            {
                timerChannel = timerGenerator();
            } else //find the next free timer
            {
                for (unsigned i = 0; timerChannel == nullptr && i < timerCnt; i++)
                {
                    timerChannel = timerPool[i]();
                }
            }
        }
        if (timerChannel == nullptr) return postError(error::noFreeChannel);

        timerChannel->begin(cb, reload, oneShot);
        if (oneShot) // naming bug, should be periodic
            timerChannel->start();

        return error::OK;
    }

    void Timer::attachErrFunc(errorFunc_t ef)
    {
        errFunc = ef;
    }

    error Timer::postError(error e)
    {
        if (errFunc != nullptr) errFunc(e);
        return e;
    }

    errorFunc_t Timer::errFunc = nullptr;
}
