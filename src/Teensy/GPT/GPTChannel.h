#pragma once

#include "../../ITimerChannel.h"
#include "GPTmap.h"
#include "core_pins.h"

namespace TeensyTimerTool
{
    class GptChannel : public ITimerChannel
    {
     public:
        inline GptChannel(IMXRT_GPT_t*, callback_t*);
        inline virtual ~GptChannel();

        inline errorCode begin(callback_t cb, float tcnt, bool periodic) override;
        inline errorCode begin(callback_t cb, uint32_t tcnt, bool periodic) override;

        inline errorCode trigger(uint32_t) override;
        inline errorCode trigger(float) override;
        inline void setPeriod(uint32_t) {}
        inline float getMaxPeriod() override;

        bool isPeriodic;

     protected:
        IMXRT_GPT_t* regs;
        uint32_t reload;
    };

    // IMPLEMENTATION ==============================================

    GptChannel::GptChannel(IMXRT_GPT_t* registers, callback_t* cbStorage)
        : ITimerChannel(cbStorage), regs(registers)
    {
    }

    errorCode GptChannel::begin(callback_t cb, uint32_t micros, bool periodic)
    {
        return begin(cb, (float)micros, periodic);
    }

    errorCode GptChannel::begin(callback_t cb, float micros, bool periodic)
    {
        isPeriodic = periodic;
        setCallback(cb);
        if (isPeriodic)
        {
            uint32_t pid_clock_mhz = (CCM_CSCMR1 & CCM_CSCMR1_PERCLK_CLK_SEL) ? 24 : (F_BUS_ACTUAL / 1000000);
            float tmp = micros * pid_clock_mhz;

            if (tmp > 0xFFFF'FFFF)
            {
                postError(errorCode::periodOverflow);
                reload = 0xFFFF'FFFE;
            } else
                reload = (uint32_t)tmp - 1;

            regs->SR = 0x3F;         // clear all interupt flags
            regs->IR = GPT_IR_OF1IE; // enable OF1 interrupt
            regs->OCR1 = reload;     // set overflow value
            regs->CR |= GPT_CR_EN;   // enable timer
        }
        return errorCode::OK;
    }

    GptChannel::~GptChannel()
    {
        regs->CR &= ~GPT_CR_EN;
        regs->IR = 0x00;
        setCallback(nullptr);
    }

    errorCode GptChannel::trigger(uint32_t delay)
    {
        return trigger((float)delay);
    }

    errorCode GptChannel::trigger(float delay) //should be optimized somehow
    {
        uint32_t pid_clock_mhz = (CCM_CSCMR1 & CCM_CSCMR1_PERCLK_CLK_SEL) ? 24 : (F_BUS_ACTUAL / 1000000);
        float tmp = delay * pid_clock_mhz;

        if (tmp > 0xFFFF'FFFF)
        {
            postError(errorCode::periodOverflow);
            reload = 0xFFFF'FFFE;
        } else
            reload = (uint32_t)tmp - 1;

        regs->SR = 0x3F;         // clear all interupt flags
        regs->IR = GPT_IR_OF1IE; // enable OF1 interrupt
        regs->OCR1 = reload ; // set overflow value
        regs->CR |= GPT_CR_EN;   // enable timer

        return errorCode::OK;
    }

    float GptChannel::getMaxPeriod()
    {
        uint32_t pid_clock_mhz = (CCM_CSCMR1 & CCM_CSCMR1_PERCLK_CLK_SEL) ? 24 : (F_BUS_ACTUAL / 1000000);
        return  (float) 0xFFFF'FFFE / pid_clock_mhz;
    }

} // namespace TeensyTimerTool