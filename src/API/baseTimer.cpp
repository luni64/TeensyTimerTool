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

    BaseTimer::BaseTimer(BaseTimer &&other) noexcept
        : timerGenerator(other.timerGenerator), timerChannel(other.timerChannel), isPeriodic(other.isPeriodic),
          prescaler(other.prescaler), hasPrescaler(other.hasPrescaler)
    {
        other.timerChannel = nullptr; // the channel moves, the generator / prescaler settings are copied
    }

    BaseTimer &BaseTimer::operator=(BaseTimer &&other) noexcept
    {
        if (this != &other)
        {
            if (timerChannel != nullptr) timerChannel->release(); // the assigned-to timer gives up its own channel
            timerGenerator     = other.timerGenerator;
            timerChannel       = other.timerChannel;
            isPeriodic         = other.isPeriodic;
            prescaler          = other.prescaler;
            hasPrescaler       = other.hasPrescaler;
            other.timerChannel = nullptr;
        }
        return *this;
    }

    BaseTimer::~BaseTimer()
    {
        if (timerChannel != nullptr)
        {
            timerChannel->release(); // don't delete directly, some channels are statically allocated
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