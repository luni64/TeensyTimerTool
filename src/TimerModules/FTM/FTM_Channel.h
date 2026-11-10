#pragma once

#include "Arduino.h"
#include "FTM_ChannelInfo.h"
#include "FTM_Info.h"
#include "ITimerChannel.h"

namespace TeensyTimerTool
{
    class FTM_Channel : public ITimerChannel
    {
     public:
        inline FTM_Channel(FTM_r_t *regs, FTM_ChannelInfo *ci);
        inline virtual ~FTM_Channel();

        inline float getMaxPeriod() const override;

        inline errorCode begin(callback_t cb, float period, bool periodic) { return doBegin(cb, period, periodic); } ///hacky improve in later version
        inline errorCode begin(callback_t cb, uint32_t period, bool periodic) { return doBegin(cb, period, periodic); };

        inline errorCode trigger(float tcnt) override FASTRUN;
        inline errorCode triggerDirect(uint32_t reload) override FASTRUN;
        inline errorCode getTriggerReload(float delay, uint32_t *reload) override;

        inline errorCode start() override;
        inline errorCode stop() override;

        inline errorCode ticksFromMicros(float micros, uint32_t *ticks) const; // nearest tick, at least one

        // inline void setPeriod(uint32_t) {}

     protected:
        FTM_ChannelInfo *ci;
        FTM_r_t *regs;
        callback_t *pCallback = nullptr;

        template <typename period_t>
        inline errorCode doBegin(callback_t cb, period_t period, bool periodic);
    };

    // IMPLEMENTATION ==============================================

    FTM_Channel::FTM_Channel(FTM_r_t *regs, FTM_ChannelInfo *channelInfo)
        : ITimerChannel(nullptr)
    {
        this->regs = regs;
        this->ci   = channelInfo;
    }

    template <typename period_t>
    errorCode FTM_Channel::doBegin(callback_t callback, period_t period, bool periodic)
    {
        stop(); // begin() on a running timer: BaseTimer::begin() restarts it
        ci->isPeriodic = periodic;
        ci->callback   = callback;
        return periodic ? ticksFromMicros(period, &ci->reload) : errorCode::OK;
    }

    errorCode FTM_Channel::start()
    {
        uint32_t cv = regs->CNT + ci->reload;        // compare value (current counter + reload)
        ci->chRegs->SC &= ~FTM_CSC_CHF;              // reset timer flag
        regs->SC &= ~FTM_SC_CLKS_MASK;               // a CV written while the clock runs isn't used at once:
        ci->chRegs->CV = cv;                         // the first callback came at the old compare value
        regs->SC |= FTM_SC_CLKS(0b01);               // (as in triggerDirect)
        ci->chRegs->SC = FTM_CSC_MSA | FTM_CSC_CHIE; // enable interrupts
        return errorCode::OK;
    }

    errorCode FTM_Channel::stop()
    {
        ci->chRegs->SC &= ~FTM_CSC_CHIE; // enable interrupts
        ci->chRegs->SC &= ~FTM_CSC_CHF;  // reset timer flag

        return errorCode::OK;
    }

    errorCode FTM_Channel::getTriggerReload(float delay, uint32_t *reload)
    {
        return ticksFromMicros(delay, reload);
    }

    errorCode FTM_Channel::trigger(float delay)
    {
        uint32_t reload;
        errorCode status = ticksFromMicros(delay, &reload);
        triggerDirect(reload);
        return status;
    }

    errorCode FTM_Channel::triggerDirect(uint32_t reload)
    {
        if (reload > 0xFFFF) reload = 0xFFFF;
        uint32_t cv = regs->CNT + reload + 1;        // calc early to minimize error
        ci->chRegs->SC &= ~FTM_CSC_CHF;              // Reset timer flag
                                                     //
        regs->SC &= ~FTM_SC_CLKS_MASK;               // need to switch off clock to immediately set new CV
        ci->chRegs->CV = cv;                         // compare value (current counter + pReload)
        regs->SC |= FTM_SC_CLKS(0b01);               // restart clock
                                                     //
        ci->chRegs->SC = FTM_CSC_MSA | FTM_CSC_CHIE; // enable interrupts

        return errorCode::OK;
    }

    float FTM_Channel::getMaxPeriod() const
    {
        return (0xFFFF * 1E-6f) / ci->ticksPerMicrosecond; // max period in seconds
    }

    // Doesn't post errors, callers return the status which the API layer posts.
    errorCode FTM_Channel::ticksFromMicros(float micros, uint32_t *ticks) const
    {
        float t = ci->ticksPerMicrosecond * micros + 0.5f;
        if (t >= 0x10000)
        {
            *ticks = 0xFFFF; // warning only, continues with the max. period
            return errorCode::periodOverflow;
        }
        *ticks = t < 1.0f ? 1 : (uint32_t)t;
        return errorCode::OK;
    }

    FTM_Channel::~FTM_Channel() // releases the channel: no more callbacks, free for the next getTimer()
    {
        stop();
        ci->callback   = nullptr;
        ci->isReserved = false;
    }

} // namespace TeensyTimerTool
