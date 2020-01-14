#pragma once

#include "C_GPTchannel.h"
#include "C_GPTmap.h"
#include <Arduino.h>

namespace TeensyTimerTool
{
    template <unsigned moduleNr>
    class C_GPT_t
    {
     public:
        static ITimerChannel* getTimer();

     protected:
        static bool isInitialized;
        static void isr();
        static void dummyIsr(){}
        static callback_t callbacks[3];
        static uint32_t reloads[3];

        // the following is caculated at compile time
        static constexpr IRQ_NUMBER_t irq = moduleNr == 0 ? IRQ_GPT1 : IRQ_GPT2;
        static constexpr volatile IMXRT_GPT_t* pGPT = (volatile IMXRT_GPT_t*)(moduleNr == 0 ? &IMXRT_GPT1 : &IMXRT_GPT2);

        static_assert(moduleNr < 2, "Wrong GPT Number");
    };

    // IMPLEMENTATION ===========================================================================

    template <unsigned moduleNr>
    ITimerChannel* C_GPT_t<moduleNr>::getTimer()
    {
        if (!isInitialized)
        {
            if (moduleNr == 0)  // compile time constant, branch will be optimized away
            {
                CCM_CCGR1 |= CCM_CCGR1_GPT1_BUS(CCM_CCGR_ON) | CCM_CCGR1_GPT1_SERIAL(CCM_CCGR_ON); // enable GPT1 module
            } else
            {
                CCM_CCGR0 |= CCM_CCGR0_GPT2_BUS(CCM_CCGR_ON) | CCM_CCGR0_GPT2_SERIAL(CCM_CCGR_ON); // enable clock
            }

            pGPT->CR = 0;
            pGPT->IR = 0;
            pGPT->CR = GPT_CR_FRR | GPT_CR_CLKSRC(0x001); // Freerunning and peripheral clock

            attachInterruptVector(irq, isr); // start
            NVIC_ENABLE_IRQ(irq);
            pGPT->CR |= GPT_CR_EN;
            isInitialized = true;

            callbacks[0] = nullptr;  // allocate first channel
            callbacks[1] = nullptr; // mark as free
            callbacks[2] = nullptr;

            return new C_GptChannel(pGPT, 0, callbacks, reloads);
        }

        for (unsigned chNr = 0; chNr < 3; chNr++)
        {
            if (callbacks[chNr] == nullptr)
            {
                //callbacks[chNr] = nullptr;  //allocate channel
                return new C_GptChannel(pGPT, chNr, callbacks, reloads);
            }
        }

        return nullptr;
    }

    template <unsigned tmoduleNr>
    void C_GPT_t<tmoduleNr>::isr()
    {
        uint32_t CNT = pGPT->CNT; // capture the counter as soon as possible to avoid frequency errors

        pGPT->CR &= ~GPT_CR_EN;   // stop counter, unfortunately the OCRx registers can not be changed while the counter is running :-(
        asm volatile("dsb");      // we need to wait until the data is actually transferred from the register to the variable (what a weird chip design)
        uint32_t SR = pGPT->SR;   // cache the SR register for efficiency

        //asm volatile("dsb");
        if (SR & GPT_SR_OF1) pGPT->OCR1 = CNT + reloads[0];  // update the overflow registers for the next cycle
        if (SR & GPT_SR_OF2) pGPT->OCR2 = CNT + reloads[1];
        if (SR & GPT_SR_OF3) pGPT->OCR3 = CNT +  reloads[2];

        pGPT->SR |= SR;           // ...and delete the flags in the status register
        pGPT->CR |= GPT_CR_EN;    // restart counter...

        if (SR & GPT_SR_OF1) callbacks[0]();  // calling the callbacks while the counter is stopped would generate a huge timing error,...
        if (SR & GPT_SR_OF2) callbacks[1]();  // ...so, lets call them after talking to the pheripherals
        if (SR & GPT_SR_OF3) callbacks[2]();
    }


    template <unsigned m>
    bool C_GPT_t<m>::isInitialized = false;

    template <unsigned m>
    callback_t C_GPT_t<m>::callbacks[3];

    template <unsigned m>
    uint32_t C_GPT_t<m>::reloads[3];

    constexpr TimerGenerator* CGPT1 = C_GPT_t<0>::getTimer;
    constexpr TimerGenerator* CGPT2 = C_GPT_t<1>::getTimer;

} // namespace TeensyTimerTool
