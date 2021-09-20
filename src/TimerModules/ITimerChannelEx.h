#pragma once

#include "../boardDef.h"
#include "../ErrorHandling/error_codes.h"
//#include "types.h"

namespace TeensyTimerTool
{
    class ITimerChannelEx
    {
     public:
        //mandatory API
        virtual errorCode stop()     = 0;
        virtual float getMaxPeriod() = 0;

        // optional API
        virtual errorCode setPrescaler(int psc) { return errorCode::notImplemented; }
        virtual ~ITimerChannelEx() {}

        virtual void tick() {} // ugly, but need to add this here since we can't do mulitple inheritance (missing dynamic casts, RTTI)
    };
} // namespace TeensyTimerTool