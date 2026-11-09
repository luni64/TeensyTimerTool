#pragma once
#include "Arduino.h"
#include "GPTmap.h"
#include "ITimerChannel.h"
#include "core_pins.h"

namespace TeensyTimerTool
{
    class GptChannel : public ITimerChannel
    {
     public:
        inline GptChannel(IMXRT_GPT_t *, callback_t *);
        virtual ~GptChannel();

        inline errorCode begin(callback_t cb, float tcnt, bool periodic) override;
        inline errorCode start() override;
        inline errorCode stop() override;

        inline errorCode trigger(float delay) override;
        inline errorCode triggerDirect(uint32_t delay) override;
        inline errorCode getTriggerReload(float delay, uint32_t *reload) override;

        inline errorCode setNextPeriod(float us) override;
        inline errorCode setPeriod(float us) override;

        inline float getMaxPeriod() const override { return getMaxMicros() / 1E6; }

        inline errorCode setPrescaler(int psc) override; // psc 0..12 (PSC_1..PSC_4096) -> prescaler: 1..4096

        bool isPeriodic;

     protected:
        inline errorCode us2ticks(float micros, uint32_t *reload) const;
        inline float getMaxMicros() const;
        inline static float perclkMHz();

        IMXRT_GPT_t *regs;
        float clock;          // counter clock in MHz (perclk / prescaler)
        uint32_t pscDiv = 1;  // prescaler divider 1..4096
    };

    // IMPLEMENTATION ==============================================

    GptChannel::GptChannel(IMXRT_GPT_t *registers, callback_t *cbStorage)
        : ITimerChannel(cbStorage), regs(registers)
    {
    }

    errorCode GptChannel::begin(callback_t cb, float period, bool periodic)
    {
        clock      = perclkMHz() / pscDiv;
        isPeriodic = periodic;
        errorCode status = errorCode::OK;
        if (isPeriodic)
        {
            status = setPeriod(period);
        }
        setCallback(cb);

        return status;
    }

    errorCode GptChannel::start()
    {
        regs->SR = 0x3F;         // clear all interrupt flags
        regs->IR = GPT_IR_OF1IE; // enable OF1 interrupt
        regs->CR |= GPT_CR_EN;   // enable timer
        return errorCode::OK;
    }

    errorCode GptChannel::stop()
    {
        regs->CR &= ~GPT_CR_EN; // disable timer
        regs->IR = 0;
        return errorCode::OK;
    }

    errorCode GptChannel::trigger(float delay) // should be optimized somehow
    {
        uint32_t reload;
        errorCode status = us2ticks(delay, &reload);
        triggerDirect(reload);
        return status;
    }

    errorCode GptChannel::triggerDirect(uint32_t reload)
    {
        regs->SR   = 0x3F;         // clear all interrupt flags
        regs->IR   = GPT_IR_OF1IE; // enable OF1 interrupt
        regs->OCR1 = reload;       // set overflow value
        regs->CR |= GPT_CR_EN;     // enable timer

        return errorCode::OK;
    }

    errorCode GptChannel::getTriggerReload(float delay, uint32_t *reload)
    {
        return us2ticks(delay, reload);
    }

    // reload = ticks - 1, rounded to the nearest tick, at least one tick.
    // Doesn't post errors, callers return the status which the API layer posts.
    errorCode GptChannel::us2ticks(float micros, uint32_t *reload) const
    {
        if (micros > getMaxMicros())
        {
            *reload = 0xFFFF'FFFE;
            return errorCode::periodOverflow;
        }
        float ticks = clock * micros + 0.5f;
        *reload     = ticks < 2.0f ? 0 : (uint32_t)ticks - 1;
        return errorCode::OK;
    }

    float GptChannel::perclkMHz()
    {
        return (CCM_CSCMR1 & CCM_CSCMR1_PERCLK_CLK_SEL) ? 24.0f : (F_BUS_ACTUAL / 1000000);
    }

    errorCode GptChannel::setPrescaler(int psc)
    {
        if (psc < PSC_1 || psc > PSC_4096) return errorCode::argument; // PSC_AUTO not supported for GPT

        pscDiv   = 1u << psc;
        regs->PR = pscDiv - 1; // takes effect immediately, prescaler counter restarts (RM 52.7.1.3)
        clock    = perclkMHz() / pscDiv;
        return errorCode::OK;
    }

    float GptChannel::getMaxMicros() const
    {
        return (float)0xFFFF'FFFE / clock;
    }

    errorCode GptChannel::setNextPeriod(float us)
    {
        return errorCode::notImplemented;
    }

    errorCode GptChannel::setPeriod(float us) // restart: in restart mode a write to OCR1 resets the counter (RM 52.5.1.1)
    {
        uint32_t reload;
        errorCode status = us2ticks(us, &reload);
        regs->OCR1       = reload;
        return status;
    }

} // namespace TeensyTimerTool