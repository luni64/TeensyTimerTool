#pragma once

#include "../IOneShotChannelEx.h"
#include "gptHelpers.h"

namespace TeensyTimerTool
{
    class GptOneShotChannel : public IOneShotChannelEx
    {
     public:
        inline GptOneShotChannel(IMXRT_GPT_t *, callback_t *);
        inline virtual ~GptOneShotChannel() override;

        inline errorCode begin(callback_t cb) override;
        inline errorCode stop() override;
        inline errorCode end() override;
        inline float getMaxPeriod() override;

        inline errorCode trigger(float) override { return errorCode::notImplemented; }

     protected:
        IMXRT_GPT_t *regs;
        uint32_t reload;
        callback_t *callback;
    };

    // INLINE IMPLEMENTATION ================================================================

    GptOneShotChannel::GptOneShotChannel(IMXRT_GPT_t *registers, callback_t *cbStorage)
    {
        regs     = registers;
        callback = cbStorage;
    }

    GptOneShotChannel::~GptOneShotChannel()
    {
        end();
    }

    errorCode GptOneShotChannel::begin(callback_t cb)
    {
        if (cb == nullptr) return errorCode::arguments;

        *callback = cb;
        reload    = 0;
        //regs->OCR1 = reload;

        return errorCode::OK;
    }

    // errorCode GptOneShotChannel::start()
    // {
    //     regs->SR = 0x3F;         // clear all interupt flags
    //     regs->IR = GPT_IR_OF1IE; // enable OF1 interrupt
    //     regs->CR |= GPT_CR_EN;   // enable timer
    //     return errorCode::OK;
    // }

    errorCode GptOneShotChannel::stop()
    {
        regs->CR &= ~GPT_CR_EN; // disable timer
        regs->IR = 0;
        return errorCode::OK;
    }

    errorCode GptOneShotChannel::end()
    {
        stop();
        *callback = nullptr;
        return GptHelpers::removeTimer(regs);
    }

    float GptOneShotChannel::getMaxPeriod()
    {
        return GptHelpers::getMaxMicros() / 1E6;
    }

} // namespace TeensyTimerTool