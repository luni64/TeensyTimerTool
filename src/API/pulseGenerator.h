#pragma once

#include "TimerModules/PULSE/PlsChannelTCK.h"
#include "ErrorHandling/error_codes.h"
#include "IPulseGenChannel.h"
#include "TimerModules/PULSE/PLS.h"
#include "boardDef.h"
#include <cstdint>

// very sketchy, needs rework

namespace TeensyTimerTool
{
    class PulseGenerator
    {
     public:
        PulseGenerator() {}

        inline errorCode begin(uint8_t pin);
        inline errorCode trigger(const PulseTrain &pulseTrain);

     protected:
        IPulseGenChannel *channel = nullptr;
    };

    // IMPLEMENTATION =====================================================================

    errorCode PulseGenerator::begin(uint8_t pin)
    {
        if (!channel) channel = PLS::getChannel(pin);
        if(channel)
        {
            return channel->begin();
        }
        return errorCode::notInitialized;
    }

    errorCode PulseGenerator::trigger(const PulseTrain &pulseTrain)
    {
        if (channel)
            return postError(channel->trigger(pulseTrain));

        return postError(errorCode::notInitialized);
    }
}; // namespace TeensyTimerTool
