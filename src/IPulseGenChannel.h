#pragma once

#include "API/pulseTrain.h"
#include "types.h"

namespace TeensyTimerTool
{
    class IPulseGenChannel
    {
     public:
        virtual errorCode begin() = 0;
        virtual errorCode trigger(const PulseTrain &pulseTrain) = 0;
        //virtual errorCode getState() = 0;
    };
}