#pragma once
//#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "config.h"
#include "tmrChannel.h"
#include <cmath>

namespace TeensyTimerTool
{
    class TmrPeriodicChannel : public IPeriodicChannelEx, public TmrChannel
    {
     public:
        inline TmrPeriodicChannel(IMXRT_TMR_CH_t *regs, callback_t *cbStorage);

        inline errorCode begin(callback_t cb, float tcnt, bool start = true) override;
        inline errorCode start() override;
        inline errorCode stop() override { return TmrChannel::stop(); } // stop timer but keep it allocated

        inline errorCode setPeriod(float us) override;

        inline errorCode end() override { return TmrChannel::end(); } // stop and release timer
        inline float getMaxPeriod() override { return TmrChannel::getMaxPeriod(); }

        inline ~TmrPeriodicChannel();
    };

    // IMPLEMENTATION ==============================================

    TmrPeriodicChannel::TmrPeriodicChannel(IMXRT_TMR_CH_t *regs, callback_t *cbStorage)
        : TmrChannel(regs, cbStorage)
    {}

    TmrPeriodicChannel::~TmrPeriodicChannel()
    {
        Serial.println("tmr dtor");
    }

    errorCode TmrPeriodicChannel::begin(callback_t cb, float tcnt, bool startImmediately)
    {
        Serial.println("tmrbegin");
        *cbStorage = cb;

        const float_t t       = microsecondToCounter(tcnt);
        const uint16_t reload = t > 0xFFFF ? 0xFFFE : (uint16_t)t - 1;

        regs->CTRL   = 0x0000;
        regs->LOAD   = 0x0000;
        regs->COMP1  = reload;
        regs->CMPLD1 = reload;
        regs->CNTR   = 0x0000;
        regs->CTRL   = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_LENGTH;

        if (startImmediately) start();
        return t > 0xFFFF ? errorCode::periodOverflow : errorCode::OK;
    }

    errorCode TmrPeriodicChannel::start()
    {
        regs->CNTR = 0x0000;

        regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
        regs->CSCTRL |= TMR_CSCTRL_TCF1EN;
        return errorCode::OK;
    }

    // void TMRChannel::_setNextPeriod(const uint16_t cnt)
    // {
    //     regs->CMPLD1 = cnt;
    // }

    // errorCode TMRChannel::_setCurrentPeriod(const uint16_t cnt)
    // {

    //     regs->COMP1 = cnt;

    //     //Do we need to wait some cycle for IP bus to update here / cache flush?
    //     //asm volatile("dsb");

    //     if (regs->CNTR > cnt)
    //     {
    //         //if counter alrready went over setted value force a triggering
    //         regs->CNTR = cnt;
    //         return errorCode::triggeredLate;
    //     }

    //     else
    //     {
    //         return errorCode::OK;
    //     }
    // }

    errorCode TmrPeriodicChannel::setPeriod(float us)
    {
        //const float_t t = microsecondToCounter(us);

        // if (t <= 0xFFFF)
        // {
        //     return _setCurrentPeriod(t);
        // } else
        // {
        //     return errorCode::periodOverflow;
        // }
        return errorCode::notImplemented;
    }

} // namespace TeensyTimerTool
