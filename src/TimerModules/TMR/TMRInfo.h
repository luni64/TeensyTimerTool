#pragma once

//#include "Arduino.h"
#include <cstdint>

struct chInfo_t
{
    const uint8_t DMA_source_cmpld1;
    const uint8_t DMA_source_cmpld2;
    // const uint8_t DMA_dest_cmpld1;
    // const uint8_t DMA_dest_cmpld2;
    IMXRT_TMR_CH_t *const registers;
};

struct moduleInfo_t
{
    IMXRT_TMR_t *const moduleRegisters;
    const chInfo_t channelInfo[4];
};

template <unsigned module, unsigned channel>
constexpr chInfo_t getChannelInfo()
{
    static_assert(module >= 1 && module <= 4, "Module number must be 1..4");
    static_assert(channel < 4, "Channel number must be 0..3");

    switch (module)
    {
        case 1:
            switch (channel)
            {
                case 0: return chInfo_t{DMAMUX_SOURCE_QTIMER1_WRITE0_CMPLD1, DMAMUX_SOURCE_QTIMER1_WRITE0_CMPLD2, &IMXRT_TMR1.CH[0]};
                case 1: return chInfo_t{DMAMUX_SOURCE_QTIMER1_WRITE1_CMPLD1, DMAMUX_SOURCE_QTIMER1_WRITE1_CMPLD2, &IMXRT_TMR1.CH[1]};
                case 2: return chInfo_t{DMAMUX_SOURCE_QTIMER1_WRITE2_CMPLD1, DMAMUX_SOURCE_QTIMER1_WRITE2_CMPLD2, &IMXRT_TMR1.CH[2]};
                case 3: return chInfo_t{DMAMUX_SOURCE_QTIMER1_WRITE3_CMPLD1, DMAMUX_SOURCE_QTIMER1_WRITE3_CMPLD2, &IMXRT_TMR1.CH[3]};
            }
            break;
        case 2:
            switch (channel)
            {
                case 0: return chInfo_t{DMAMUX_SOURCE_QTIMER2_WRITE0_CMPLD1, DMAMUX_SOURCE_QTIMER2_WRITE0_CMPLD2, &IMXRT_TMR2.CH[0]};
                case 1: return chInfo_t{DMAMUX_SOURCE_QTIMER2_WRITE1_CMPLD1, DMAMUX_SOURCE_QTIMER2_WRITE1_CMPLD2, &IMXRT_TMR2.CH[1]};
                case 2: return chInfo_t{DMAMUX_SOURCE_QTIMER2_WRITE2_CMPLD1, DMAMUX_SOURCE_QTIMER2_WRITE2_CMPLD2, &IMXRT_TMR2.CH[2]};
                case 3: return chInfo_t{DMAMUX_SOURCE_QTIMER2_WRITE3_CMPLD1, DMAMUX_SOURCE_QTIMER2_WRITE3_CMPLD2, &IMXRT_TMR2.CH[3]};
            }
            break;
        case 3:
            switch (channel)
            {
                case 0: return chInfo_t{DMAMUX_SOURCE_QTIMER3_WRITE0_CMPLD1, DMAMUX_SOURCE_QTIMER3_WRITE0_CMPLD2, &IMXRT_TMR3.CH[0]};
                case 1: return chInfo_t{DMAMUX_SOURCE_QTIMER3_WRITE1_CMPLD1, DMAMUX_SOURCE_QTIMER3_WRITE1_CMPLD2, &IMXRT_TMR3.CH[1]};
                case 2: return chInfo_t{DMAMUX_SOURCE_QTIMER3_WRITE2_CMPLD1, DMAMUX_SOURCE_QTIMER3_WRITE2_CMPLD2, &IMXRT_TMR3.CH[2]};
                case 3: return chInfo_t{DMAMUX_SOURCE_QTIMER3_WRITE3_CMPLD1, DMAMUX_SOURCE_QTIMER3_WRITE3_CMPLD2, &IMXRT_TMR3.CH[3]};
            }
            break;
        case 4:
            switch (channel)
            {
                case 0: return chInfo_t{DMAMUX_SOURCE_QTIMER4_WRITE0_CMPLD1, DMAMUX_SOURCE_QTIMER4_WRITE0_CMPLD2, &IMXRT_TMR4.CH[0]};
                case 1: return chInfo_t{DMAMUX_SOURCE_QTIMER4_WRITE1_CMPLD1, DMAMUX_SOURCE_QTIMER4_WRITE1_CMPLD2, &IMXRT_TMR4.CH[1]};
                case 2: return chInfo_t{DMAMUX_SOURCE_QTIMER4_WRITE2_CMPLD1, DMAMUX_SOURCE_QTIMER4_WRITE2_CMPLD2, &IMXRT_TMR4.CH[2]};
                case 3: return chInfo_t{DMAMUX_SOURCE_QTIMER4_WRITE3_CMPLD1, DMAMUX_SOURCE_QTIMER4_WRITE3_CMPLD2, &IMXRT_TMR4.CH[3]};
            }
            break;

        default:
            break;
    }
    return chInfo_t{0, 0};
}

template <unsigned module>
constexpr moduleInfo_t getModuleInfo()
{
    //IMXRT_TMR_t *moduleRegisters;

    return moduleInfo_t{
        module == 1   ? &IMXRT_TMR1
        : module == 2 ? &IMXRT_TMR2
        : module == 3 ? &IMXRT_TMR3
                      : &IMXRT_TMR4,
        {
            getChannelInfo<module, 0>(),
            getChannelInfo<module, 1>(),
            getChannelInfo<module, 2>(),
            getChannelInfo<module, 3>(),
        }};
};

// template <unsigned module>
// const int TMRInfo<module>::channelInfo[1]{0};
