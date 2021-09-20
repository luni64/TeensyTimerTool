#pragma once

#include "../ITimerChannelEx.h"
#include "../IPeriodicChannel.h"

#include "Arduino.h"

namespace TeensyTimerTool
{
    class TckChannelBase
    {
     public:
        template <typename counter_t> static counter_t getCycleCounter();
        template <typename counter_t> static counter_t getMaxCounter();
        template <typename counter_t> static counter_t microsecondToCounter(const float us);
        template <typename counter_t> static float getMaxPeriod();
        template <typename counter_t> static float counterToMicrosecond(counter_t p);

        uint32_t lastCyCnt;
        uint32_t curHIGH;
    };

    template<typename counter_t>
    float TckChannelBase::getMaxPeriod()
    {
        return counterToMicrosecond<counter_t>(getMaxCounter<counter_t>()) / 1E6;
    }

    // template <typename counter>
    // counter::counter_t TckChannelBase::microsecondToCounter(const float us)
    // {
    //     return (counter_t)us * (F_CPU_ACTUAL / 1E6f);
    // }

    // template <typename counter_t>
    // float TckChannelBase::counterToMicrosecond(counter_t cnt)
    // {
    //     return cnt * (1E6f / F_CPU_ACTUAL);
    // }
} // namespace TeensyTimerTool