#pragma once
#include "../../ITimerChannel.h"
//#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "config.h"
#include "imxrt.h"
#include <cmath>

namespace TeensyTimerTool
{
    static_assert(TMR_DEFAULT_PSC >= PSC_1 && TMR_DEFAULT_PSC <= PSC_128, "TMR_DEFAULT_PSC (config) must be one of PSC_1 ... PSC_128");

    class TMRChannel : public ITimerChannel
    {
     public:
        inline TMRChannel(IMXRT_TMR_CH_t *regs, callback_t *cbStorage);
        inline virtual ~TMRChannel();

        inline errorCode begin(callback_t cb, float tcnt, bool periodic) override;
        inline errorCode start() override;
        inline errorCode stop() override;

        inline errorCode trigger(float tcnt) override;
        inline float getMaxPeriod() const override;
        double getPeriod() const override { return (regs->CMPLD1 + 1.0) * pscValue / 150E6; } // CMPLD1: latest period set, COMP1 + 1 ticks
        double getResolution() const override { return pscValue / 150E6; }
        float getRemainingTime() const override // counts up 0..COMP1; TCF1EN: callback pending
        {
            if (!(regs->CSCTRL & TMR_CSCTRL_TCF1EN)) return 0;
            return (regs->COMP1 + 1.0f - regs->CNTR) * pscValue / 150E6f;
        }

        inline errorCode setPeriod(float us) override;
        inline errorCode setPeriodKeepElapsed(float us) override;
        inline errorCode setNextPeriod(float us) override;
        inline errorCode setPrescaler(int psc) override; // psc 0..7 (PSC_1..PSC_128) -> prescaler: 1..128

     protected:
        IMXRT_TMR_CH_t *regs;
        callback_t **pCallback = nullptr;
        float pscValue;
        uint32_t pscBits;

        inline errorCode us2Ticks(const float us, uint16_t *ticks) const;
        inline float_t counterToMicrosecond(const float_t cnt) const;
    };

    // IMPLEMENTATION ==============================================

    TMRChannel::TMRChannel(IMXRT_TMR_CH_t *regs, callback_t *cbStorage)
        : ITimerChannel(cbStorage)
    {
        this->regs = regs;
        setPrescaler(TMR_DEFAULT_PSC);
    }

    TMRChannel::~TMRChannel()
    {
        regs->CTRL = 0x0000; // stop timer and mark it free
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

    errorCode TMRChannel::begin(callback_t cb, float period, bool periodic)
    {
        uint16_t reload;
        errorCode status = us2Ticks(period, &reload);

        regs->CTRL   = 0x0000;
        regs->LOAD   = 0x0000;
        regs->COMP1  = reload;
        regs->CMPLD1 = reload;
        regs->CNTR   = 0x0000;
        regs->CSCTRL = TMR_CSCTRL_CL1(1);
        setCallback(cb);

        if (!periodic)
            regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_ONCE | TMR_CTRL_LENGTH;

        else
            regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_LENGTH;

        // don't start() here, BaseTimer::begin() starts the timer unless begin(..., false) was requested
        return status;
    }

    errorCode TMRChannel::trigger(float us) // quick and dirty, should be optimized
    {
        // const float_t t = us2Ticks(tcnt);
        // uint16_t reload = t > 0xFFFF ? 0xFFFF : (uint16_t)t;

        uint16_t reload;
        errorCode status = us2Ticks(us, &reload);

        regs->CTRL   = 0x0000;
        regs->LOAD   = 0x0000;
        regs->COMP1  = reload;
        regs->CMPLD1 = reload;
        regs->CNTR   = 0x0000;

        regs->CSCTRL &= ~TMR_CSCTRL_TCF1;
        regs->CSCTRL |= TMR_CSCTRL_TCF1EN;

        regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_ONCE | TMR_CTRL_LENGTH;

        return status;
    }

    errorCode TMRChannel::setPrescaler(int psc) // psc 0..7 -> prescaler: 1..128
    {
        if (psc < PSC_1 || psc > PSC_128) return errorCode::argument; // PSC_AUTO not supported for TMR

        pscValue = 1 << psc;
        pscBits  = 0b1000 | psc;

        // Running timer: switch the clock source too, so that later setPeriod() / setNextPeriod() calls
        // (computed with the new pscValue) match the counter clock. The current reload stays in ticks.
        if (regs->CTRL != 0)
        {
            regs->CTRL = (regs->CTRL & ~TMR_CTRL_PCS(0b1111)) | TMR_CTRL_PCS(pscBits);
        }
        return errorCode::OK;
    }

    float TMRChannel::getMaxPeriod() const
    {
        return pscValue / 150'000'000.0f * 0xFFFF; // max reload 0xFFFE -> 0xFFFF ticks
    }

    errorCode TMRChannel::setNextPeriod(float us)
    {
        uint16_t reload;
        errorCode status = us2Ticks(us, &reload);
        regs->CMPLD1     = reload;
        return status;
    }

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

    errorCode TMRChannel::setPeriod(float us) // restart: the new period starts now
    {
        uint16_t newReload;
        errorCode status = us2Ticks(us, &newReload);

        noInterrupts();
        regs->CMPLD1 = newReload;
        regs->COMP1  = newReload;
        regs->CNTR   = 0x0000;
        regs->CSCTRL &= ~TMR_CSCTRL_TCF1; // discard a compare event of the old period
        interrupts();

        return status;
    }

    errorCode TMRChannel::setPeriodKeepElapsed(float us) // behaviour of setPeriod() before v1.5
    {
        uint16_t newReload;
        errorCode status = us2Ticks(us, &newReload);

        regs->CMPLD1 = newReload;     // counter will load this value to COMP1 at next trigger
                                      //
        noInterrupts();               // interrupting this code could lead to wrong cntr settings
        if (regs->CNTR > newReload)   // already too late for new period
            regs->CNTR = regs->COMP1; //   -> force trigger;  will also load COMP1 with value from CMPLD1
        else                          // not too late
            regs->COMP1 = newReload;  //   -> change current compare value to new one (counter _might_ be > newReload in between.. watch and fix if necessary)
        interrupts();

        return status;
    }

    // Returns the compare value for a period of 'us'. The counter runs 0..COMP1, i.e. a period is COMP1 + 1 ticks.
    // Rounded to the nearest tick, at least one tick.
    errorCode TMRChannel::us2Ticks(const float us, uint16_t *reload) const
    {
        constexpr uint16_t maxReload = 0xFFFE;

        float ticks = us * 150.0f / pscValue + 0.5f;
        if (ticks >= maxReload + 2.0f)
        {
            *reload = maxReload;
            return errorCode::periodOverflow;
        }
        *reload = ticks < 2.0f ? 0 : (uint16_t)ticks - 1;
        return errorCode::OK;
    }

    float_t TMRChannel::counterToMicrosecond(const float_t cnt) const
    {
        return cnt * pscValue / 150.0f;
    }

} // namespace TeensyTimerTool
