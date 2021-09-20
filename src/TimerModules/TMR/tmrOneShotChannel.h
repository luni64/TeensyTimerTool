#pragma once

//#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "config.h"
#include "tmrChannel.h"

namespace TeensyTimerTool
{
    class TmrOneShotChannel : public TmrChannel, public IOneShotChannelEx
    {
     public:
        inline TmrOneShotChannel(IMXRT_TMR_CH_t *regs, callback_t *cbStorage);

        inline errorCode begin(callback_t cb) override;
        inline errorCode trigger(float tcnt) override;                  // start timer
        inline errorCode stop() override { return TmrChannel::stop(); } // stop timer but keep it allocated
        inline float getMaxPeriod() override { return TmrChannel::getMaxPeriod(); }

        errorCode getTriggerReload(float delay, uint32_t *reload)
        {
            uint32_t r = microsecondToCounter(delay);

            *reload = r > 0xFFFF ? 0xFFFF : r;
            return r > 0xFFFF ? errorCode::periodOverflow : errorCode::OK;
        };

     protected:

    };

    // IMPLEMENTATION ==============================================

    TmrOneShotChannel::TmrOneShotChannel(IMXRT_TMR_CH_t *regs, callback_t *cbStorage)
        : TmrChannel(regs, cbStorage)
    {}

    errorCode TmrOneShotChannel::begin(callback_t cb)
    {
        *cbStorage = cb;
        regs->CTRL = 0x0000;
        return errorCode::OK;
    }

    errorCode TmrOneShotChannel::trigger(float tcnt)
    {
        const float_t t = microsecondToCounter(tcnt);
        uint16_t reload = t > 0xFFFF ? 0xFFFF : (uint16_t)t - 1;

        regs->CTRL   = 0x0000;
        regs->LOAD   = 0x0000;
        regs->COMP1  = reload;
        regs->CMPLD1 = reload;
        regs->CNTR   = 0x0000;

        regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
        regs->CSCTRL |= TMR_CSCTRL_TCF1EN;

        regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_ONCE | TMR_CTRL_LENGTH;

        return errorCode::OK;
    }
} // namespace TeensyTimerTool
