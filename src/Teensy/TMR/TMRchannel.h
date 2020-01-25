#pragma once

#include "../../ITimerChannel.h"

namespace TeensyTimerTool
{
class TmrChannel : public ITimerChannel
{
public:
    inline TmrChannel(IMXRT_TMR_CH_t *regs, callback_t *cbStorage);
    inline virtual ~TmrChannel();

    inline void begin(callback_t cb, unsigned tcnt, bool periodic)
    {
        double t = tcnt * (150.0 / 128.0);
        uint16_t reload = t > 0xFFFF ? 0xFFFF : (uint16_t)t - 1;
        
        regs->CTRL = 0x0000;
        regs->LOAD = 0x0000;
        regs->COMP1 = reload;
        regs->CMPLD1 = reload;
        regs->CNTR = 0x0000;
        setCallback(cb);
        regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
        regs->CSCTRL |= TMR_CSCTRL_TCF1EN;

        if (!periodic)
            regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(0b1111) | TMR_CTRL_ONCE | TMR_CTRL_LENGTH;

        else
            regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(0b1111) | TMR_CTRL_LENGTH;
    }

    inline void trigger(uint32_t tcnt) // quick and dirty, should be optimized
    {
        double t = tcnt * (150.0 / 128.0);
        uint16_t reload = t > 0xFFFF ? 0xFFFF : (uint16_t)t;

        regs->CTRL = 0x0000;
        regs->LOAD = 0x0000;
        regs->COMP1 = reload;
        regs->CMPLD1 = reload;
        regs->CNTR = 0x0000;

        regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
        regs->CSCTRL |= TMR_CSCTRL_TCF1EN;

        regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(0b1111) | TMR_CTRL_ONCE | TMR_CTRL_LENGTH;
    }

    inline void setPeriod(uint32_t) {}

protected:
    IMXRT_TMR_CH_t *regs;
    callback_t **pCallback = nullptr;
};

// IMPLEMENTATION ==============================================

TmrChannel::TmrChannel(IMXRT_TMR_CH_t *regs, callback_t *cbStorage)
    : ITimerChannel(cbStorage)
{
    this->regs = regs;
}

TmrChannel::~TmrChannel()
{
}

} // namespace TeensyTimerTool
