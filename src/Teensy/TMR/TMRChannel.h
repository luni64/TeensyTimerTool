#pragma once
#include "../../ITimerChannel.h"
#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "config.h"
#include "imxrt.h"

namespace TeensyTimerTool
{
    class TMRChannel : public ITimerChannel
    {
     public:
        inline TMRChannel(IMXRT_TMR_CH_t* regs, callback_t* cbStorage);
        inline virtual ~TMRChannel();

        inline errorCode begin(callback_t cb, uint32_t tcnt, bool periodic) override;
        inline errorCode begin(callback_t cb, float tcnt, bool periodic) override;

        inline errorCode trigger(uint32_t tcnt) override;
        inline errorCode trigger(float tcnt) override;

        inline float getMaxPeriod() override;
        inline void setPeriod(uint32_t) override {}
        inline void setPrescaler(uint32_t psc); // psc 0..7 -> prescaler: 1..128

     protected:
        IMXRT_TMR_CH_t* regs;
        callback_t** pCallback = nullptr;
        float pscValue;
        uint32_t pscBits;
    };

    // IMPLEMENTATION ==============================================

    TMRChannel::TMRChannel(IMXRT_TMR_CH_t* regs, callback_t* cbStorage)
        : ITimerChannel(cbStorage)
    {
        this->regs = regs;
        setPrescaler(TMR_DEFAULT_PSC);
    }

    TMRChannel::~TMRChannel()
    {
    }

    errorCode TMRChannel::begin(callback_t cb, uint32_t tcnt, bool periodic)
    {
        return begin(cb, (float)tcnt, periodic);
    }

    errorCode TMRChannel::begin(callback_t cb, float tcnt, bool periodic)
    {
        float t = tcnt * (150.0f / pscValue);
        uint16_t reload;
        if(t > 0xFFFF)
        {
            postError(errorCode::periodOverflow);
            reload = 0xFFFE;
        }
        else reload = (uint16_t)t - 1;

        regs->CTRL = 0x0000;
        regs->LOAD = 0x0000;
        regs->COMP1 = reload;
        regs->CMPLD1 = reload;
        regs->CNTR = 0x0000;
        setCallback(cb);
        regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
        regs->CSCTRL |= TMR_CSCTRL_TCF1EN;

        if (!periodic)
            regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_ONCE | TMR_CTRL_LENGTH;

        else
            regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_LENGTH;

        return t > 0xFFFF ? errorCode::periodOverflow : errorCode::OK;
    }

    errorCode TMRChannel::trigger(uint32_t tcnt)
    {
        return trigger((float)tcnt);
    }

    errorCode TMRChannel::trigger(float tcnt) // quick and dirty, should be optimized
    {
        float t = tcnt * (150.0f / pscValue);
        uint16_t reload = t > 0xFFFF ? 0xFFFF : (uint16_t)t;

        regs->CTRL = 0x0000;
        regs->LOAD = 0x0000;
        regs->COMP1 = reload;
        regs->CMPLD1 = reload;
        regs->CNTR = 0x0000;

        regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
        regs->CSCTRL |= TMR_CSCTRL_TCF1EN;

        regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_ONCE | TMR_CTRL_LENGTH;

        return errorCode::OK;
    }

    void TMRChannel::setPrescaler(uint32_t psc) // psc 0..7 -> prescaler: 1..128
    {
        pscValue = 1 << (psc & 0b0111);
        pscBits = 0b1000 | (psc & 0b0111);
    }

    float TMRChannel::getMaxPeriod()
    {
        return pscValue / 150.0f * 0xFFFE;
    }

} // namespace TeensyTimerTool
