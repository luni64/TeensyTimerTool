#pragma once

#include "RtcChannel.h"

namespace TeensyTimerTool
{
    namespace RTC_t
    {
        callback_t callback = nullptr;
        inline RtcChannel channel(&callback);

        void isr()
        {
            SNVS_HPSR |= 0b11; // reset interrupt flag
            if (callback != nullptr) callback();
            asm volatile("DSB");
        }

        ITimerChannel *getTimer()
        {
            static bool initialized = false;
            if (!initialized)
            {
                attachInterruptVector(IRQ_SNVS_IRQ, isr);
                NVIC_ENABLE_IRQ(IRQ_SNVS_IRQ);
            }
            return &channel;
        }

    } // namespace RTC_t

} // namespace TeensyTimerTool
