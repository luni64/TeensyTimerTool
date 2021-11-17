#pragma once
#include "IPulseGenChannel.h"
#include "PlsChannelTCK.h"

// sketchy, needs rework
// needs to handle pins connected to flexPWM timers as well

struct pwm_pin_info_struct
{
    uint8_t type;          // 0=no pwm, 1=flexpwm, 2=quad
    uint8_t module;        // 0-3, 0-3
    uint8_t channelNumber; // 0=X, 1=A, 2=B
    uint8_t muxval;        //
};
extern const struct pwm_pin_info_struct pwm_pin_info[];

namespace TeensyTimerTool
{
    class PLS
    {
     public:
        static IPulseGenChannel *getChannel(uint8_t pin)
        {
            auto pinInfo = pwm_pin_info[pin];
            if (pinInfo.type == 2)
            {
                return new PulseChannelTCK(pin);
            }
            else
                return nullptr;        }
    };
} // namespace TeensyTimerTool