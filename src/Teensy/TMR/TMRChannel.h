#pragma once
#include "../../ITimerChannel.h"
//#include "Arduino.h"
#include <cmath>
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
       
        inline errorCode begin(callback_t cb, float tcnt, bool periodic) override;
        inline errorCode start() override;
        inline errorCode stop() override;
        
        inline errorCode trigger(float tcnt) override;
        inline float getMaxPeriod() const override;

        inline errorCode setPeriod(float us) override;
        inline void setPrescaler(uint32_t psc); // psc 0..7 -> prescaler: 1..128

     protected:
        IMXRT_TMR_CH_t* regs;
        callback_t** pCallback = nullptr;
        float pscValue;
        uint32_t pscBits;

        inline float_t microsecondToCounter(const float_t us) const;
        inline float_t counterToMicrosecond(const float_t cnt) const;     
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

    errorCode TMRChannel::start()
    {
        regs->CNTR = 0x0000;
        
        regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
        regs->CSCTRL |= TMR_CSCTRL_TCF1EN;
        return errorCode::OK;
    }

    errorCode TMRChannel::stop()
    {
        regs->CSCTRL &= ~TMR_CSCTRL_TCF1EN;
        return errorCode::OK;
    }

   
    errorCode TMRChannel::begin(callback_t cb, float tcnt, bool periodic)
    {
        const float_t t = microsecondToCounter(tcnt);
        uint16_t reload;
        if (t > 0xFFFF)
        {
            postError(errorCode::periodOverflow);
            reload = 0xFFFE;
        } else
        {
            reload = (uint16_t)t - 1;
        }

        regs->CTRL = 0x0000;
        regs->LOAD = 0x0000;
        regs->COMP1 = reload;
        regs->CMPLD1 = reload;
        regs->CNTR = 0x0000;
        setCallback(cb);

        if (!periodic)
            regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_ONCE | TMR_CTRL_LENGTH;

        else
            regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_LENGTH;

        start();
        return t > 0xFFFF ? errorCode::periodOverflow : errorCode::OK;
    }

    
    errorCode TMRChannel::trigger(float tcnt) // quick and dirty, should be optimized
    {
        const float_t t = microsecondToCounter(tcnt);
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

    float TMRChannel::getMaxPeriod() const
    {
        return pscValue / 150'000'000.0f * 0xFFFE;
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

    errorCode TMRChannel::setPeriod(float us)
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


    float_t TMRChannel::microsecondToCounter(const float_t us) const
    {
        return us * 150.0f / pscValue;
    }

    float_t TMRChannel::counterToMicrosecond(const float_t cnt) const
    {
        return cnt * pscValue / 150.0f;
    }

} // namespace TeensyTimerTool
