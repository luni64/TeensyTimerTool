#pragma once

#include "TmrChannel.h"

//#include <Arduino.h>
//#include <cstdint>

namespace TeensyTimerTool
{
    template <unsigned moduleNr>
    class TMR_t
    {
     public:
        static ITimerChannel* getTimer();

     protected:
        static bool isInitialized;
        static void isr();
        static callback_t callbacks[4];

        // the following is calculated at compile time
        static constexpr IRQ_NUMBER_t irq = moduleNr == 0 ? IRQ_QTIMER1 : moduleNr == 1 ? IRQ_QTIMER2 : moduleNr == 2 ? IRQ_QTIMER3 : IRQ_QTIMER4;
        static constexpr IMXRT_TMR_t* pTMR = moduleNr == 0 ? &IMXRT_TMR1 : moduleNr == 1 ? &IMXRT_TMR2 : moduleNr == 2 ? &IMXRT_TMR3 : &IMXRT_TMR4;
        static constexpr IMXRT_TMR_CH_t* pCH0 = &pTMR->CH[0];
        static constexpr IMXRT_TMR_CH_t* pCH1 = &pTMR->CH[1];
        static constexpr IMXRT_TMR_CH_t* pCH2 = &pTMR->CH[2];
        static constexpr IMXRT_TMR_CH_t* pCH3 = &pTMR->CH[3];

        static_assert(moduleNr < 4, "Module number < 4 required");
    };

    // IMPLEMENTATION ==================================================================

    template <unsigned moduleNr>
    ITimerChannel* TMR_t<moduleNr>::getTimer()
    {
        //Serial.printf("getTimer(%d)\n", moduleNr);

        if (!isInitialized)
        {
            for (unsigned chNr = 0; chNr < 4; chNr++)
            {
                pTMR->CH[chNr].CTRL = 0x0000;
                callbacks[chNr] = nullptr;
            }
            attachInterruptVector(irq, isr); // start
            NVIC_ENABLE_IRQ(irq);
            isInitialized = true;
            return new TmrChannel(pCH0, &callbacks[0]);
        }

        for (unsigned chNr = 0; chNr < 4; chNr++) 
        {
            IMXRT_TMR_CH_t* pCh = &pTMR->CH[chNr];
            if (pCh->CTRL == 0x0000)
            {
                return new TmrChannel(pCh, &callbacks[chNr]);
            }
        }

        return nullptr;
    }

    template <unsigned m> 
    void TMR_t<m>::isr()
    {
        // no loop to gain some time by avoiding indirections and pointer calculations
        if (callbacks[0] != nullptr && pCH0->CSCTRL & TMR_CSCTRL_TCF1)
        {
            pCH0->CSCTRL &= ~TMR_CSCTRL_TCF1;
            callbacks[0]();
        }

        if (callbacks[1] != nullptr && pCH1->CSCTRL & TMR_CSCTRL_TCF1)
        {
            pCH1->CSCTRL &= ~TMR_CSCTRL_TCF1;
            callbacks[1]();
        }

        if (callbacks[2] != nullptr && pCH2->CSCTRL & TMR_CSCTRL_TCF1)
        {
            pCH2->CSCTRL &= ~TMR_CSCTRL_TCF1;
            callbacks[2]();
        }

        if (callbacks[3] != nullptr && pCH3->CSCTRL & TMR_CSCTRL_TCF1)
        {
            pCH3->CSCTRL &= ~TMR_CSCTRL_TCF1;
            callbacks[3]();
        }        
    }

    template <unsigned m>
    bool TMR_t<m>::isInitialized = false;

    template <unsigned m>
    callback_t TMR_t<m>::callbacks[4];

    constexpr TimerGenerator* TMR1 = TMR_t<0>::getTimer;
    constexpr TimerGenerator* TMR2 = TMR_t<1>::getTimer;
    constexpr TimerGenerator* TMR3 = TMR_t<2>::getTimer;
    constexpr TimerGenerator* TMR4 = TMR_t<3>::getTimer;

} // namespace TeensyTimerTool
