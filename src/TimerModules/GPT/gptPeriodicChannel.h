#pragma once

#include "../IPeriodicChannelEx.h"
#include "gptHelpers.h"

namespace TeensyTimerTool
{
    class GptPeriodicChannel : public IPeriodicChannelEx
    {
     public:
        inline GptPeriodicChannel(IMXRT_GPT_t *, callback_t *);
        inline virtual ~GptPeriodicChannel() override;

        inline errorCode begin(callback_t cb, float period, bool startImmediately = true) override;
        inline errorCode setPeriod(float) override;
        inline errorCode start() override;
        inline errorCode stop() override;
        inline errorCode end() override;
        inline float getMaxPeriod() override;

     protected:
        IMXRT_GPT_t *regs;
        uint32_t reload;
        callback_t *callback;
    };

    // INLINE IMPLEMENTATION ================================================================

    GptPeriodicChannel::GptPeriodicChannel(IMXRT_GPT_t *registers, callback_t *cbStorage)
    {
        regs     = registers;
        callback = cbStorage;
    }

    GptPeriodicChannel::~GptPeriodicChannel()
    {
        end();
    }

    errorCode GptPeriodicChannel::begin(callback_t cb, float period, bool startImmediately)
    {
        if (cb == nullptr || period < 0.1) return errorCode::arguments;

        *callback  = cb;
        reload     = GptHelpers::microsecondToCycles(period);
        regs->OCR1 = reload;

        if (startImmediately) start();

        return errorCode::OK;
    }

    errorCode GptPeriodicChannel::setPeriod(float)
    {
        return errorCode::notImplemented;
    }

    errorCode GptPeriodicChannel::start()
    {
        regs->SR = 0x3F;         // clear all interupt flags
        regs->IR = GPT_IR_OF1IE; // enable OF1 interrupt
        regs->CR |= GPT_CR_EN;   // enable timer
        return errorCode::OK;
    }

    errorCode GptPeriodicChannel::stop()
    {
        regs->CR &= ~GPT_CR_EN; // disable timer
        regs->IR = 0;
        return errorCode::OK;
    }

    errorCode GptPeriodicChannel::end()
    {
        stop();
        *callback = nullptr;
        return GptHelpers::removeTimer(regs);
    }

    float GptPeriodicChannel::getMaxPeriod()
    {
        return GptHelpers::getMaxMicros() / 1E6;
    }

} // namespace TeensyTimerTool