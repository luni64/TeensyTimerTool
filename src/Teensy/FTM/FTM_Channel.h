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

        inline errorCode begin(callback_t cb, uint32_t tcnt, bool periodic);
        inline errorCode trigger(uint32_t tcnt) FASTRUN;

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
        ci->reload = std::min(0xFFFFu, FTM_Info<0>::MicrosToReload(tcnt));
        ci->callback = callback;

        ci->chRegs->CV = regs->CNT + ci->reload;     // compare value (current counter + pReload)
        ci->chRegs->SC &= ~FTM_CSC_CHF;              // Reset timer flag
        ci->chRegs->SC = FTM_CSC_MSA | FTM_CSC_CHIE; // enable interrupts
        return errorCode::OK;
    }

    errorCode FTM_Channel::trigger(const uint32_t tcnt)
    {
        //ci->chRegs->SC &= ~FTM_CSC_CHF;              // Reset timer flag
        ci->chRegs->SC = FTM_CSC_MSA | FTM_CSC_CHIE; // enable interrupts
        uint32_t cv = regs->CNT + std::min(0xFFFFu, FTM_Info<0>::MicrosToReload(tcnt));

        regs->SC = 0;        // need to switch of clock to immediately set new CV
        ci->chRegs->CV = cv; // compare value (current counter + pReload)
        regs->SC = FTM_SC_CLKS(0b01) | FTM_SC_PS(FTM_Info<0>::prescale);

        return errorCode::OK;
    }

    FTM_Channel::~FTM_Channel()
    {
    }

} // namespace TeensyTimerTool
