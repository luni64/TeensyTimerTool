#pragma once

#include "ITimerChannelEx.h"

#include "../types.h"

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
        virtual errorCode setNextPeriod(float microSeconds) { return errorCode::notImplemented; };
        virtual uint32_t getPeriod() {  return 0; }
    };
} // namespace TeensyTimerTool