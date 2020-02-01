#pragma once
#include "../../ITimerChannel.h"
#include "Arduino.h"
#include "config.h"
#include "imxrt.h"

namespace TeensyTimerTool
{
    class TMRChannel : public ITimerChannel
    {
     public:
        inline TMRChannel(IMXRT_TMR_CH_t* regs, callback_t* cbStorage);
        inline virtual ~TMRChannel();

        inline errorCode begin(callback_t cb, unsigned tcnt, bool periodic)
        {
            float t = tcnt * (150.0f / pscValue);            
            uint16_t reload = t > 0xFFFF ? 0xFFFF : (uint16_t)t - 1;
            Serial.println(reload);

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

            return t > 0xFFFF ?  errorCode::periodOverflow : errorCode::OK;
        }

        inline void trigger(uint32_t tcnt) // quick and dirty, should be optimized
        {
            double t = tcnt * (150.0 / pscValue);
            uint16_t reload = t > 0xFFFF ? 0xFFFF : (uint16_t)t;

            regs->CTRL = 0x0000;
            regs->LOAD = 0x0000;
            regs->COMP1 = reload;
            regs->CMPLD1 = reload;
            regs->CNTR = 0x0000;

            regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
            regs->CSCTRL |= TMR_CSCTRL_TCF1EN;

            regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_ONCE | TMR_CTRL_LENGTH;
        }

        inline void setPeriod(uint32_t) {}

        inline void setPrescaler(uint32_t psc) // psc 0..7 -> prescaler: 1..128
        {
            pscValue = 1 << (psc & 0b0111);
            pscBits = 0b1000 | (psc & 0b0111);

            Serial.printf("sp psc:%u pscval %f, pscbits:%X\n", psc, pscValue, pscBits);
        }

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

} // namespace TeensyTimerTool
