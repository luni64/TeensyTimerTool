#include "baseTimer.h"
//#include "Arduino.h"
#include "types.h"

namespace TeensyTimerTool
{

    BaseTimer::BaseTimer(TimerGenerator *generator, bool periodic)
        : timerGenerator(generator)
    {
        this->timerGenerator = generator;
        this->timerChannel   = nullptr;
        this->isPeriodic     = periodic;
    }

    BaseTimer::~BaseTimer()
    {
        if (timerChannel != nullptr)
        {
            delete timerChannel;
        }
    }

    errorCode BaseTimer::setPrescaler(int psc)
    {
        prescaler    = psc;
        hasPrescaler = true;

        // no channel yet -> will be applied in begin()
        if (timerChannel == nullptr) return errorCode::OK;

        return postError(timerChannel->setPrescaler(psc));
    }

} // namespace TeensyTimerTool