#pragma once

#include "../../ITimerChannel.h"
#include "Arduino.h"
#include "FTM_ChannelInfo.h"
#include "FTM_Info.h"

namespace TeensyTimerTool
{
    class FTM_Channel : public ITimerChannel
    {
     public:
        inline FTM_Channel(FTM_r_t* regs, FTM_ChannelInfo* ci);
        inline virtual ~FTM_Channel();

        inline float getMaxPeriod() override;
        inline errorCode begin(callback_t cb, uint32_t tcnt, bool periodic);
        inline errorCode trigger(uint32_t tcnt) FASTRUN;

        inline uint16_t ticksFromMicros(float micros);
        inline void setPeriod(uint32_t) {}

     protected:
        FTM_ChannelInfo* ci;
        FTM_r_t* regs;
        callback_t* pCallback = nullptr;
    };

    // IMPLEMENTATION ==============================================

    FTM_Channel::FTM_Channel(FTM_r_t* regs, FTM_ChannelInfo* channelInfo)
        : ITimerChannel(nullptr)
    {
        this->regs = regs;
        this->ci = channelInfo;
    }

    errorCode FTM_Channel::begin(callback_t callback, uint32_t tcnt, bool periodic)
    {
        ci->isPeriodic = periodic;
        ci->reload = ticksFromMicros(tcnt);
        ci->callback = callback;

        ci->chRegs->CV = regs->CNT + ci->reload;               // compare value (current counter + pReload)
        ci->chRegs->SC &= ~FTM_CSC_CHF;                        // reset timer flag
        ci->chRegs->SC = FTM_CSC_MSA | FTM_CSC_CHIE;           // enable interrupts
        return errorCode::OK;
    }

    errorCode FTM_Channel::trigger(const uint32_t micros)
    {
        uint32_t cv = regs->CNT + ticksFromMicros(micros) + 1; // calc early to minimize error
        ci->chRegs->SC &= ~FTM_CSC_CHF;                        // Reset timer flag

        regs->SC &= ~FTM_SC_CLKS_MASK;                         // need to switch off clock to immediately set new CV
        ci->chRegs->CV = cv;                                   // compare value (current counter + pReload)
        regs->SC |= FTM_SC_CLKS(0b01);                         // restart clock

        ci->chRegs->SC = FTM_CSC_MSA | FTM_CSC_CHIE;           // enable interrupts
        return errorCode::OK;
    }

    float FTM_Channel::getMaxPeriod()
    {
        return  (0xFFFF * 1E-6f) / ci->ticksPerMicrosecond;    // max period in seconds
    }

    uint16_t FTM_Channel::ticksFromMicros(float micros)
    {
        uint32_t rl = ci->ticksPerMicrosecond * micros;
        if (rl > 0xFFFF)
        {
            postError(errorCode::periodOverflow);              // warning only, continues with clipped value
            rl = 0xFFFF;
        }
        return rl;
    }

    FTM_Channel::~FTM_Channel()
    {
    }

} // namespace TeensyTimerTool
