#pragma once

#include "PITChannel.h"

namespace TeensyTimerTool
{
    class PIT_t
    {
     public:
        inline static ITimerChannel *getTimer();

     protected:
        static bool isInitialized;
        static void isr();
        static PITChannel channel[4];
    };

    // IMPLEMENTATION ===========================================================================

    ITimerChannel *PIT_t::getTimer()
    {
        if (!isInitialized)
        {
            isInitialized = true;

            CCM_CCGR1 |= CCM_CCGR1_PIT(CCM_CCGR_ON);
            PIT_MCR = 1;

            if (USE_GPT_PIT_150MHz)                       // timer clock setting from config.h
                CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL; // FBus (usually 150MHz)
            else
                CCM_CSCMR1 |= CCM_CSCMR1_PERCLK_CLK_SEL; // 24MHz

            attachInterruptVector(IRQ_PIT, isr);
            NVIC_ENABLE_IRQ(IRQ_PIT);
        }

        for (unsigned i = 0; i < 4; i++)
        {
            if (channel[i].callback == nullptr) return &channel[i];
        }
        return nullptr;
    }

    inline void PIT_t::isr()
    {
        // All channels share this interrupt, the TFLG registers tell which one fired. Reading a TFLG costs ~100 CPU
        // cycles (slow peripheral bus), so the callback (RAM) is checked first: only channels in use are read.
        // A free channel (callback == nullptr) has its interrupt disabled (release() -> stop()), so its flag can't
        // keep the interrupt active. The DSB at the end makes sure the flag resets reached the PIT before the ISR
        // returns, otherwise the ISR would be called again for the same event.
        if (channel[0].callback != nullptr && IMXRT_PIT_CHANNELS[0].TFLG)
        {
            IMXRT_PIT_CHANNELS[0].TFLG = 1;
            channel[0].isr();
        }
        if (channel[1].callback != nullptr && IMXRT_PIT_CHANNELS[1].TFLG)
        {
            IMXRT_PIT_CHANNELS[1].TFLG = 1;
            channel[1].isr();
        }
        if (channel[2].callback != nullptr && IMXRT_PIT_CHANNELS[2].TFLG)
        {
            IMXRT_PIT_CHANNELS[2].TFLG = 1;
            channel[2].isr();
        }
        if (channel[3].callback != nullptr && IMXRT_PIT_CHANNELS[3].TFLG)
        {
            IMXRT_PIT_CHANNELS[3].TFLG = 1;
            channel[3].isr();
        }

        asm volatile("dsb"); //wait until register changes propagated through the cache
    }
} // namespace TeensyTimerTool
