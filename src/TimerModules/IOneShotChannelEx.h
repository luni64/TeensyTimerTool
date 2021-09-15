#pragma once

#include "ITimerChannelEx.h"
#include "../types.h"
#include "../ErrorHandling/error_codes.h"

namespace TeensyTimerTool
{
    class IOneShotChannelEx : public ITimerChannelEx
    {
     public:
        // mandatory API
        virtual errorCode begin(callback_t callback) = 0;
        virtual errorCode trigger(float delay)       = 0;

        // optional API
        virtual errorCode triggerDirect(uint32_t reload) { return postError(errorCode::notImplemented); };
        virtual errorCode triggerDirect(uint64_t reload) { return postError(errorCode::notImplemented); };
        virtual errorCode getTriggerReload(float delay, uint32_t *reload) { return postError(errorCode::notImplemented); };
        virtual errorCode getTriggerReload(float delay, uint64_t *reload) { return postError(errorCode::notImplemented); };
    };
} // namespace TeensyTimerTool