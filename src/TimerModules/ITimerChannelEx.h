#pragma once

#include "types.h"
#include "../boardDef.h"

namespace TeensyTimerTool
{
    class ITimerChannelEx
    {
     public:
        //mandatory API
        virtual errorCode stop()     = 0;
        virtual errorCode end()      = 0;
        virtual float getMaxPeriod() = 0;

        // optional API
        virtual errorCode setPrescaler(int psc) { return postError(errorCode::notImplemented); }
        virtual ~ITimerChannelEx(){};

        virtual void tick() { postError(errorCode::notImplemented); }  // ugly, but need to add this here since we can't do mulitple inheritance (missing dynamic casts, RTTI)
    };
} // namespace TeensyTimerTool