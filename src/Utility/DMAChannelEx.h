#pragma once

#include "CallbackStateHelper.h"
#include "DMAChannel.h"

namespace TeensyTimerTool
{
    class PulseChannelTCK;
    
    class DMAChannelEx : public DMAChannel
    {
     public:
        using state_t    = PulseChannelTCK *;
        using callback_t = void(state_t);
        using DMAChannel::DMAChannel;

        void attachInterrupt(callback_t *callback, state_t state)
        {
            stateHelper.callbacks[channel] = callback;
            stateHelper.states[channel]    = state;
            DMAChannel::attachInterrupt(stateHelper.relays[channel]);
        }

     protected:
        CallbackStateHelper<DMAChannelEx, DMA_NUM_CHANNELS> stateHelper;
    };
} // namespace TeensyTimerTool