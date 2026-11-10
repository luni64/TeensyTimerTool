#pragma once
#include "Arduino.h"
#include "GPTmap.h"
#include "ITimerChannel.h"
#include "core_pins.h"

namespace TeensyTimerTool
{
    // Efficiency notes (also valid for GPT.h)
    // The GPT registers sit behind the slow peripheral bus: every register access, read or write, costs ~100 CPU
    // cycles with the default 24 MHz timer clock (~44 cycles with USE_GPT_PIT_150MHz), a RAM access ~5 cycles.
    // A register read additionally waits until all pending register writes have reached the peripheral.
    // Therefore the time critical paths (periodic ISR, trigger, start) only write registers, constant values without
    // read-modify-write. The one-shot ISR reads the status once (see GPT_t::isr), the getters read registers.
    //
    // The timer runs in restart mode: the counter resets at the compare, periods are exact and the ISR doesn't need
    // to touch the compare register. Consequence: setNextPeriod() and setPeriod(.., keepElapsed) are not supported,
    // since every write to OCR1 resets the counter (RM 52.5.1.1). Free-run mode could do it, but its ISR needs to
    // read and check the counter at every compare, which costs ~4 times the cost of the plain ISR (measured).
    class GptChannel final : public ITimerChannel // final: internal calls of virtual functions are direct calls
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

        inline errorCode setPeriod(float us) override;

        inline float getMaxPeriod() const override { return getMaxMicros() / 1E6; }
        double getPeriod() const override { return (regs->OCR1 + 1.0) / (clock * 1E6); } // restart mode: OCR1 + 1 ticks
        double getResolution() const override { return 1.0 / (clock * 1E6); }
        float getRemainingTime() const override // restart mode: counts up 0..OCR1; one-shots are disabled after firing
        {
            if (!(regs->CR & GPT_CR_EN)) return 0;
            return (regs->OCR1 + 1.0f - regs->CNT) / (clock * 1E6f);
        }

        inline errorCode setPrescaler(int psc) override; // psc 0..12 (PSC_1..PSC_4096) -> prescaler: 1..4096

        bool isPeriodic;
        static constexpr uint32_t CR_STOPPED = GPT_CR_CLKSRC(0x001) | GPT_CR_ENMOD; // restart mode, counter reset when enabled, perclk
        static constexpr uint32_t CR_RUNNING = CR_STOPPED | GPT_CR_EN;

     protected:
        inline errorCode us2ticks(float micros, uint32_t *reload) const;
        inline float getMaxMicros() const;
        inline static float perclkMHz();

        IMXRT_GPT_t *regs;
        IRQ_NUMBER_t irq;
        float clock;         // counter clock in MHz (perclk / prescaler)
        uint32_t pscDiv = 1; // prescaler divider 1..4096
    };

    // IMPLEMENTATION ==============================================

    GptChannel::GptChannel(IMXRT_GPT_t *registers, callback_t *cbStorage)
        : ITimerChannel(cbStorage), regs(registers), irq(registers == (IMXRT_GPT_t *)&IMXRT_GPT1 ? IRQ_GPT1 : IRQ_GPT2)
    {
    }

    errorCode GptChannel::begin(callback_t cb, float period, bool periodic)
    {
        clock            = perclkMHz() / pscDiv;
        isPeriodic       = periodic;
        errorCode status = errorCode::OK;
        if (isPeriodic)
        {
            status = setPeriod(period);
        }
        setCallback(cb);

        return status;
    }

    errorCode GptChannel::start() // a running timer continues (EN 1->1: no counter reset)
    {
        regs->SR = 0x3F;         // clear all interrupt flags
        regs->IR = GPT_IR_OF1IE; // enable OF1 interrupt
        regs->CR = CR_RUNNING;   // enable timer, counter starts at 0 (ENMOD)
        return errorCode::OK;
    }

    errorCode GptChannel::stop()
    {
        regs->CR = CR_STOPPED;
        regs->IR = 0;
        regs->SR = 0x3F;
        // A compare just before the IR write may have left the interrupt pending: wait until the writes reached
        // the GPT (otherwise the still active request would set the pending bit again), then discard it.
        asm volatile("dsb");
        NVIC_CLEAR_PENDING(irq);
        return errorCode::OK;
    }

    errorCode GptChannel::trigger(float delay)
    {
        uint32_t reload;
        errorCode status = us2ticks(delay, &reload);
        triggerDirect(reload);
        return status;
    }

    errorCode GptChannel::triggerDirect(uint32_t reload)
    {
        regs->OCR1 = reload;       // first: resets the counter, i.e. a running trigger can't fire anymore
        regs->SR   = 0x3F;         // clear all interrupt flags
        regs->IR   = GPT_IR_OF1IE; // enable OF1 interrupt
        regs->CR   = CR_RUNNING;   // enable timer (counter reset by ENMOD if it was stopped)
        // A previous trigger which expired just before may have left its interrupt pending. The ISR detects this
        // (no OF1 flag) and doesn't stop the timer. Clearing the pending interrupt here would need a DSB, i.e.
        // waiting until the writes above reached the GPT: trigger() would take 0.9 µs instead of 0.35 µs.
        return errorCode::OK;
    }

    errorCode GptChannel::getTriggerReload(float delay, uint32_t *reload)
    {
        return us2ticks(delay, reload);
    }

    // reload = ticks - 1, rounded to the nearest tick, at least one tick.
    // Doesn't post errors, callers return the status which the API layer posts.
    // Hot path of trigger(): one multiply, no division (a float division costs 14 cycles)
    errorCode GptChannel::us2ticks(float micros, uint32_t *reload) const
    {
        float ticks = clock * micros + 0.5f;
        if (ticks >= 4294967296.0f) // 2^32: more than 0xFFFF'FFFF ticks (max reload 0xFFFF'FFFE)
        {
            *reload = 0xFFFF'FFFE;
            return errorCode::periodOverflow;
        }
        *reload     = ticks < 2.0f ? 0 : (uint32_t)ticks - 1;
        return errorCode::OK;
    }

    float GptChannel::perclkMHz()
    {
        return (CCM_CSCMR1 & CCM_CSCMR1_PERCLK_CLK_SEL) ? 24.0f : F_BUS_ACTUAL / 1E6f; // F_BUS needn't be whole MHz (e.g. 75.6)
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

    errorCode GptChannel::setPeriod(float us) // restart: in restart mode a write to OCR1 resets the counter (RM 52.5.1.1)
    {
        uint32_t reload;
        errorCode status = us2ticks(us, &reload);
        regs->OCR1       = reload;
        return status;
    }

} // namespace TeensyTimerTool
