#pragma once

#include "TMRChannel.h"

namespace TeensyTimerTool
{

    template <unsigned moduleNr>
    class TMR_t
    {
     public:
        static ITimerChannelEx *makeTimer(TimerType);

     protected:
        static bool isInitialized;
        static void isr();
        static callback_t callbacks[4];

        // the following is calculated at compile time
        static constexpr IRQ_NUMBER_t irq = moduleNr == 0 ? IRQ_QTIMER1
                                          : moduleNr == 1 ? IRQ_QTIMER2
                                          : moduleNr == 2 ? IRQ_QTIMER3
                                                          : IRQ_QTIMER4;
        static IMXRT_TMR_t *const pTMR;
        static IMXRT_TMR_CH_t *const pCH0;
        static IMXRT_TMR_CH_t *const pCH1;
        static IMXRT_TMR_CH_t *const pCH2;
        static IMXRT_TMR_CH_t *const pCH3;

        static_assert(moduleNr < 4, "Module number < 4 required");
    };

    // IMPLEMENTATION ==================================================================

    template <unsigned moduleNr>
    ITimerChannelEx *TMR_t<moduleNr>::makeTimer(TimerType t)
    {
        if (!isInitialized)
        {
            for (unsigned chNr = 0; chNr < 4; chNr++)
            {
                pTMR->CH[chNr].CTRL = 0x0000;
                callbacks[chNr]     = nullptr;
            }
            attachInterruptVector(irq, isr); // start
            NVIC_ENABLE_IRQ(irq);
            isInitialized = true;
        }

        for (unsigned chNr = 0; chNr < 4; chNr++)
        {
            IMXRT_TMR_CH_t *pCh = &pTMR->CH[chNr];
            if (pCh->CTRL == 0x0000)
            {
                switch (t)
                {
                    // case TimerType::periodic: return new TmrPeriodicChannel(pCh, &callbacks[chNr]);
                    // case TimerType::oneShot: return new TmrOneShotChannel(pCh, &callbacks[chNr]);
                    default: return nullptr;
                }
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
        asm volatile("dsb"); //wait until register changes propagated through the cache
    }

    template <unsigned m> bool TMR_t<m>::isInitialized = false;
    template <unsigned m> callback_t TMR_t<m>::callbacks[4];
    template <unsigned m> IMXRT_TMR_CH_t *const TMR_t<m>::pCH0 = &pTMR->CH[0];
    template <unsigned m> IMXRT_TMR_CH_t *const TMR_t<m>::pCH1 = &pTMR->CH[1];
    template <unsigned m> IMXRT_TMR_CH_t *const TMR_t<m>::pCH2 = &pTMR->CH[2];
    template <unsigned m> IMXRT_TMR_CH_t *const TMR_t<m>::pCH3 = &pTMR->CH[3];

    template <unsigned m> IMXRT_TMR_t *const TMR_t<m>::pTMR = m == 0 ? &IMXRT_TMR1
                                                            : m == 1 ? &IMXRT_TMR2
                                                            : m == 2 ? &IMXRT_TMR3
                                                                     : &IMXRT_TMR4;

} // namespace TeensyTimerTool
