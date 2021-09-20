#pragma once
#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "types.h"

namespace TeensyTimerTool
{
    class TmrChannel
    {
     public:
        inline TmrChannel(IMXRT_TMR_CH_t *regs, callback_t *cbStorage);
        inline virtual ~TmrChannel();

        inline errorCode setPrescaler(int psc); // psc 0..7 -> prescaler: 1..128
        inline errorCode stop() const;
        inline float getMaxPeriod() const;

     protected:
        IMXRT_TMR_CH_t *regs;
        callback_t *cbStorage;
        float pscValue;
        uint32_t pscBits;



        inline float microsecondToCounter(const float us) const;
        inline float counterToMicrosecond(const float cnt) const;
    };

    // IMPLEMENTATION ==============================================

    TmrChannel::TmrChannel(IMXRT_TMR_CH_t *regs, callback_t *cbStorage)
    {
        setPrescaler(TMR_DEFAULT_PSC);
        this->regs      = regs;
        this->cbStorage = cbStorage;
    }

    TmrChannel::~TmrChannel()
    {
        Serial.println("tmrchannel::dtor");
        stop();
        regs->CTRL = 0x0000; // stop timer and mark it free
    }

    errorCode TmrChannel::stop() const
    {
        regs->CSCTRL &= ~TMR_CSCTRL_TCF1EN; // disable interrupt
        return errorCode::OK;
    }

    errorCode TmrChannel::setPrescaler(int psc) // psc 0..7 -> prescaler: 1..128
    {
        pscValue = 1 << (psc & 0b0111);
        pscBits  = 0b1000 | (psc & 0b0111);
        return errorCode::OK;
    }

    float TmrChannel::getMaxPeriod() const
    {
        return pscValue / 150'000'000.0f * 0xFFFE;
    }

    float TmrChannel::microsecondToCounter(const float us) const
    {
        return us * 150.0f / pscValue;
    }

    float TmrChannel::counterToMicrosecond(const float cnt) const
    {
        return cnt * pscValue / 150.0f;
    }

} // namespace TeensyTimerTool