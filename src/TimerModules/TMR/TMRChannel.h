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

    class TMRChannel final : public ITimerChannel // final: internal calls of virtual functions are direct calls
    {
     public:
        inline TMRChannel(IMXRT_TMR_CH_t *regs, callback_t *cbStorage, bool *oneShotStorage);
        inline virtual ~TMRChannel();

        inline errorCode begin(callback_t cb, float tcnt, bool periodic) override;
        inline errorCode start() override;
        inline errorCode stop() override;

        inline errorCode trigger(float tcnt) override;
        inline errorCode triggerDirect(uint32_t reload) override; // reload 0..0xFFFE (compare value, period = reload + 1 ticks)
        inline errorCode getTriggerReload(float delay, uint32_t *reload) override;
        inline float getMaxPeriod() const override;
        double getPeriod() const override { return (regs->CMPLD1 + 1.0) / (ticksPerUs * 1E6); } // CMPLD1: latest period set, COMP1 + 1 ticks
        double getResolution() const override { return 1.0 / (ticksPerUs * 1E6); }
        float getRemainingTime() const override // counts up 0..COMP1; TCF1EN: callback pending
        {
            if (!(regs->CSCTRL & TMR_CSCTRL_TCF1EN)) return 0;
            return (regs->COMP1 + 1.0f - regs->CNTR) / (ticksPerUs * 1E6f);
        }

        inline errorCode setPeriod(float us) override;
        inline errorCode setPeriodKeepElapsed(float us) override;
        inline errorCode setNextPeriod(float us) override;
        inline errorCode setPrescaler(int psc) override; // psc 0..7 (PSC_1..PSC_128) -> prescaler: 1..128

     protected:
        IMXRT_TMR_CH_t *regs;
        callback_t **pCallback = nullptr;
        bool *pOneShot; // read by the ISR (a CTRL read would cost ~100 CPU cycles)
        float pscValue;
        float ticksPerUs; // F_BUS [MHz] / pscValue, avoids a float division in trigger() / setPeriod()
        inline void updateClock() { ticksPerUs = F_BUS_ACTUAL / 1E6f / pscValue; } // begin(), setPrescaler(): F_BUS follows F_CPU
        uint32_t pscBits;
        uint16_t nextReload = 0; // RAM copy of CMPLD1, the latest period set; start() makes it the current one

        inline errorCode us2Ticks(const float us, uint16_t *ticks) const;
        inline void startTrigger(uint16_t reload);
        inline float_t counterToMicrosecond(const float_t cnt) const;
    };

    // IMPLEMENTATION ==============================================

    TMRChannel::TMRChannel(IMXRT_TMR_CH_t *regs, callback_t *cbStorage, bool *oneShotStorage)
        : ITimerChannel(cbStorage), pOneShot(oneShotStorage)
    {
        this->regs = regs;
        setPrescaler(TMR_DEFAULT_PSC);
    }

    TMRChannel::~TMRChannel()
    {
        regs->CTRL = 0x0000; // stop timer and mark it free
    }

    errorCode TMRChannel::start() // the first period is the latest one set (COMP1 may still hold an older one)
    {
        regs->COMP1  = nextReload;
        regs->CNTR   = 0x0000;
        regs->CSCTRL = TMR_CSCTRL_CL1(1) | TMR_CSCTRL_TCF1EN; // clears TCF1, no read-modify-write (slow peripheral reads)
        return errorCode::OK;
    }

    errorCode TMRChannel::stop()
    {
        regs->CSCTRL = TMR_CSCTRL_CL1(1); // disable the interrupt, clear TCF1
        return errorCode::OK;
    }

    errorCode TMRChannel::begin(callback_t cb, float period, bool periodic)
    {
        updateClock(); // the TMR runs on F_BUS (IPG clock): 150 MHz at 600 MHz, but e.g. 132 MHz at 528 MHz
        uint16_t reload;
        errorCode status = us2Ticks(period, &reload);

        regs->CTRL   = 0x0000;
        regs->LOAD   = 0x0000;
        regs->COMP1  = reload;
        regs->CMPLD1 = reload;
        nextReload   = reload;
        regs->CNTR   = 0x0000;
        regs->CSCTRL = TMR_CSCTRL_CL1(1);
        *pOneShot    = !periodic;
        setCallback(cb);

        if (!periodic)
            regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_ONCE | TMR_CTRL_LENGTH;

        else
            regs->CTRL = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_LENGTH;

        // don't start() here, BaseTimer::begin() starts the timer unless begin(..., false) was requested
        return status;
    }

    // trigger() / triggerDirect(): register writes only (see TMR_t::isr). trigger() computes the reload after
    // the first write: the CPU stalls when a peripheral write is still pending, the conversion runs in this time.
    errorCode TMRChannel::trigger(float us)
    {
        regs->CTRL = 0x0000; // stop
        uint16_t reload;
        errorCode status = us2Ticks(us, &reload);
        startTrigger(reload);
        return status;
    }

    errorCode TMRChannel::triggerDirect(uint32_t reload) // reload 0..0xFFFE
    {
        errorCode status = errorCode::OK;
        if (reload > 0xFFFE)
        {
            reload = 0xFFFE;
            status = errorCode::periodOverflow;
        }
        regs->CTRL = 0x0000; // stop
        startTrigger(reload);
        return status;
    }

    errorCode TMRChannel::getTriggerReload(float delay, uint32_t *reload)
    {
        uint16_t r;
        errorCode status = us2Ticks(delay, &r);
        *reload          = r;
        return status;
    }

    void TMRChannel::startTrigger(uint16_t reload) // timer stopped (CTRL = 0)
    {
        regs->COMP1  = reload; // LOAD is 0 since begin()
        regs->CMPLD1 = reload;
        regs->CNTR   = 0x0000;
        regs->CSCTRL = TMR_CSCTRL_CL1(1) | TMR_CSCTRL_TCF1EN; // clears TCF1
        regs->CTRL   = TMR_CTRL_CM(1) | TMR_CTRL_PCS(pscBits) | TMR_CTRL_ONCE | TMR_CTRL_LENGTH;
    }

    errorCode TMRChannel::setPrescaler(int psc) // psc 0..7 -> prescaler: 1..128
    {
        if (psc < PSC_1 || psc > PSC_128) return errorCode::argument; // PSC_AUTO not supported for TMR

        pscValue   = 1 << psc;
        updateClock();
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
        return 0xFFFF / (ticksPerUs * 1E6f); // max reload 0xFFFE -> 0xFFFF ticks
    }

    errorCode TMRChannel::setNextPeriod(float us)
    {
        uint16_t reload;
        errorCode status = us2Ticks(us, &reload);
        regs->CMPLD1     = reload;
        nextReload       = reload;
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
        nextReload   = newReload;
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
        nextReload   = newReload;     //
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

        float ticks = us * ticksPerUs + 0.5f;
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
        return cnt / ticksPerUs;
    }

} // namespace TeensyTimerTool
