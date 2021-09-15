#pragma once

#include "ITimerChannelEx.h"

namespace TeensyTimerTool
{
    class IPeriodicChannelEx : public ITimerChannelEx
    {
     public:
        // mandatory API
        virtual errorCode begin(callback_t callback, float period, bool startImmediately = true) = 0;
        virtual errorCode setPeriod(float microSeconds)            = 0;
        virtual errorCode start()                                  = 0;

        // optional API
        virtual errorCode setNextPeriod(float microSeconds) { return postError(errorCode::notImplemented); };
        virtual uint32_t getPeriod() { postError(errorCode::notImplemented); return 0; }
    };
} // namespace TeensyTimerTool