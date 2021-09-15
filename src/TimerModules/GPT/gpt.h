#pragma once

#include "../ITimerChannelEx.h"
#include "Arduino.h"
#include "gptOneShotChannel.h"
#include "gptPeriodicChannel.h"

namespace TeensyTimerTool
{
    template <unsigned moduleNr>
    class GPT_t
    {
     public:
        static ITimerChannelEx *makeTimer(TimerType);
        static void removeTimer();

     protected:
        static bool isInitialized;
        static void isr();
        static callback_t callback;
        static ITimerChannelEx *channel;

        // the following is calculated at compile time
        static constexpr IRQ_NUMBER_t irq = moduleNr == 0 ? IRQ_GPT1 : IRQ_GPT2;
        static IMXRT_GPT_t *const pGPT;
        static_assert(moduleNr < 2, "Wrong GPT Number");
    };

    // IMPLEMENTATION ===========================================================================

    template <unsigned moduleNr>
    IMXRT_GPT_t *const GPT_t<moduleNr>::pGPT = reinterpret_cast<IMXRT_GPT_t *>(moduleNr == 0 ? &IMXRT_GPT1 : &IMXRT_GPT2);

    template <unsigned moduleNr>
    ITimerChannelEx *GPT_t<moduleNr>::makeTimer(TimerType timerType)
    {
        if (channel != nullptr) return nullptr; // we only have one channel

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
        pGPT->CR = GPT_CR_CLKSRC(0x001) | GPT_CR_ENMOD;                                        // stopped, restart mode and peripheral clock source

        attachInterruptVector(irq, isr);
        NVIC_ENABLE_IRQ(irq);

        switch (timerType)
        {
            case TimerType::periodic:
                channel = new GptPeriodicChannel(pGPT, &callback);
                break;
            case TimerType::oneShot:
                channel = new GptOneShotChannel(pGPT, &callback);
                break;
            default:
                channel = nullptr;
        };
        return channel;
    }

    template <unsigned tmoduleNr>
    void GPT_t<tmoduleNr>::isr()
    {
        pGPT->SR = 0x3F;     // reset all interrupt flags
        callback();          // we only enabled the OF1 interrupt-> no need to find out which interrupt was actually called
        asm volatile("dsb"); // wait until register changes propagated through the cache
    }

    template <unsigned tmoduleNr>
    void GPT_t<tmoduleNr>::removeTimer()
    {
        Serial.printf("end %d\n", tmoduleNr);
        NVIC_DISABLE_IRQ(irq);
        pGPT->CR      = 0;
        callback      = nullptr;
        //isInitialized = false;
    }

    //template <unsigned m> bool GPT_t<m>::isInitialized       = false;
    template <unsigned m> callback_t GPT_t<m>::callback      = nullptr;
    template <unsigned m> ITimerChannelEx *GPT_t<m>::channel = nullptr;
} // namespace TeensyTimerTool