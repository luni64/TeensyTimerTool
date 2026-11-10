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
        double getPeriod() const override { return nextTicks / (clock * 1E6); } // latest period set
        double getResolution() const override { return 1.0 / (clock * 1E6); }
        float getRemainingTime() const override // counter runs up to the compare value OCR1; one-shots are disabled after firing
        {
            if (!(regs->CR & GPT_CR_EN)) return 0;
            return ((uint32_t)(regs->OCR1 - regs->CNT) + 1.0f) / (clock * 1E6f);
        }

        inline errorCode setPrescaler(int psc) override; // psc 0..12 (PSC_1..PSC_4096) -> prescaler: 1..4096

        bool isPeriodic;
        static constexpr uint32_t CR_STOPPED = GPT_CR_CLKSRC(0x001) | GPT_CR_ENMOD | GPT_CR_FRR; // free-run, reset on enable, perclk
        inline void nextCompare() // called by the ISR at each compare (periodic)
        {
            if (nextTicks == 1) // the compare can't be set one tick ahead (gives 3 ticks), restart the counter instead
            {
                restart(0);
                return;
            }
            uint32_t ocr = regs->OCR1 + nextTicks;
            regs->OCR1   = ocr;
            if ((int32_t)(ocr - regs->CNT) <= 0) regs->OCR1 = regs->CNT + 2; // period shorter than the ISR latency:
        }                                                                       // as soon as possible, not after a counter wrap

     protected:
        inline errorCode us2ticks(float micros, uint32_t *reload) const;
        inline void restart(uint32_t reload); // counter -> 0, first compare after reload + 1 ticks
        uint32_t nextTicks = 1;               // length of the following periods in ticks
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
        stop(); // begin() on a running timer: start() restarts it
        clock      = perclkMHz() / pscDiv;
        isPeriodic = periodic;
        errorCode status = errorCode::OK;
        if (isPeriodic)
        {
            uint32_t reload;
            status     = us2ticks(period, &reload);
            nextTicks  = reload + 1;
            regs->OCR1 = reload;
        }
        setCallback(cb);

        return status;
    }

    // The GPT runs in free-run mode: the counter counts on after a compare and the ISR advances OCR1 by
    // the next period (exact periods, setNextPeriod). With ENMOD, enabling the timer resets the counter to 0.
    void GptChannel::restart(uint32_t reload)
    {
        regs->CR   = CR_STOPPED;             // disable (no read-modify-write: saves a slow peripheral read)
        regs->OCR1 = reload;                 // first compare after reload + 1 ticks
        regs->SR   = 0x3F;                   // clear all interrupt flags
        regs->IR   = GPT_IR_OF1IE;
        regs->CR   = CR_STOPPED | GPT_CR_EN; // enable, counter starts at 0 (ENMOD)
    }

    errorCode GptChannel::start()
    {
        restart(nextTicks - 1);
        return errorCode::OK;
    }

    errorCode GptChannel::stop()
    {
        regs->CR = CR_STOPPED; // disable timer
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
        nextTicks = reload + 1;
        restart(reload); // the ISR disables the timer after the compare
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

    errorCode GptChannel::setNextPeriod(float us) // takes effect at the next compare (ISR)
    {
        uint32_t reload;
        errorCode status = us2ticks(us, &reload);
        nextTicks        = reload + 1;
        return status;
    }

    errorCode GptChannel::setPeriod(float us) // restart: the new period starts now
    {
        uint32_t reload;
        errorCode status = us2ticks(us, &reload);
        nextTicks        = reload + 1;
        if (regs->CR & GPT_CR_EN)
            restart(reload);
        else
            regs->OCR1 = reload; // begin(..., false): start() restarts
        return status;
    }

} // namespace TeensyTimerTool