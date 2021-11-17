#pragma once

#include "API/pulseTrain.h"
#include "IPulseGenChannel.h"
#include "TimerModules/TMR/TMRInfo.h"
#include "Utility/DMAChannelEx.h"

namespace TeensyTimerTool
{
    class PulseChannelTCK : public IPulseGenChannel
    {
        //using sharedDMABuffer = PulseTrain::sharedDMABuffer;

     public:
        PulseChannelTCK(uint8_t pin);
        errorCode begin();
        errorCode trigger(const PulseTrain &pulseTrain);

     protected:
        static void DMA_ISRH(PulseChannelTCK *THIS);
        static void DMA_ISRL(PulseChannelTCK *THIS);

        IMXRT_TMR_CH_t *regs;
        uint8_t DMA_source_CMPLD1, DMA_source_CMPLD2;
        const uint8_t pin;

        DMAChannelEx dmaChnLo, dmaChnHi;
        sharedDMABuffer bufLo, bufHi;
    };
} // namespace TeensyTimerTool