#pragma once

#include "GPTChannel.h"
//#include "Arduino.h"

namespace TeensyTimerTool
{
    template <unsigned moduleNr>
    class GPT_t
    {
     public:
        static ITimerChannel *getTimer();
        static void end();

     protected:
        static bool isInitialized;
        static void isr();
        static callback_t callback;
        static GptChannel *channel;

        // the following is calculated at compile time
        static constexpr IRQ_NUMBER_t irq = moduleNr == 0 ? IRQ_GPT1 : IRQ_GPT2;
        static IMXRT_GPT_t *const pGPT;
        static_assert(moduleNr < 2, "Wrong GPT Number");
    };

    // IMPLEMENTATION ===========================================================================

    template <unsigned moduleNr>
    IMXRT_GPT_t *const GPT_t<moduleNr>::pGPT = reinterpret_cast<IMXRT_GPT_t *>(moduleNr == 0 ? &IMXRT_GPT1 : &IMXRT_GPT2);

    template <unsigned moduleNr>
    ITimerChannel *GPT_t<moduleNr>::getTimer()
    {
        if (!isInitialized)
        {
            isInitialized = true;

            if (moduleNr == 0)                                                                     // GPT1 clock settings
                CCM_CCGR1 |= CCM_CCGR1_GPT1_BUS(CCM_CCGR_ON) | CCM_CCGR1_GPT1_SERIAL(CCM_CCGR_ON); //
            else                                                                                   // GPT2
                CCM_CCGR0 |= CCM_CCGR0_GPT2_BUS(CCM_CCGR_ON) | CCM_CCGR0_GPT2_SERIAL(CCM_CCGR_ON); //
                                                                                                   //
            if (USE_GPT_PIT_150MHz)                                                                // timer clock setting from config.h
                CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL;                                          // use F_BUS
            else                                                                                   //
                CCM_CSCMR1 |= CCM_CSCMR1_PERCLK_CLK_SEL;                                           // use 24MHz
                                                                                                   //
            pGPT->CR = GptChannel::CR_STOPPED;                                                     // stopped, free-run mode (counter reset when enabled), peripheral clock
            pGPT->PR = 0;                                                                          // prescaler 1, might be left over from a previous timer

            attachInterruptVector(irq, isr);
            NVIC_ENABLE_IRQ(irq);

            channel = new GptChannel(pGPT, &callback);
            return channel;
        }
        return nullptr;
    }

    template <unsigned tmoduleNr>
    void GPT_t<tmoduleNr>::isr()
    {
        uint32_t sr = pGPT->SR;
        pGPT->SR    = 0x3F; // reset all interrupt flags
        // A latched interrupt can arrive after stop() / while the channel is released: no compare flag,
        // no callback -> ignore (was a call through a null callback -> hard fault)
        if ((sr & GPT_SR_OF1) && callback != nullptr && channel != nullptr)
        {
            if (!channel->isPeriodic)
                pGPT->CR = GptChannel::CR_STOPPED; // stop timer in one shot mode
            else                        //
                channel->nextCompare(); // free-run mode: compare value of the next period
            callback();
        }
        asm volatile("dsb"); // wait until register changes propagated through the cache
    }

    template <unsigned tmoduleNr>
    void GPT_t<tmoduleNr>::end()
    {
        //Serial.printf("end %d\n", tmoduleNr);
        NVIC_DISABLE_IRQ(irq);
        pGPT->CR      = 0;
        pGPT->IR      = 0;
        pGPT->SR      = 0x3F;
        NVIC_CLEAR_PENDING(irq);
        callback      = nullptr;
        channel       = nullptr;
        isInitialized = false;
    }

    template <unsigned m>
    bool GPT_t<m>::isInitialized = false;

    template <unsigned m>
    callback_t GPT_t<m>::callback = nullptr;

    template <unsigned m>
    GptChannel *GPT_t<m>::channel = nullptr;
} // namespace TeensyTimerTool