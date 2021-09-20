#pragma once

#include "../IOneShotChannel.h"
#include "../IPeriodicChannel.h"
#include "gptHelpers.h"


namespace TeensyTimerTool
{
    template <unsigned channelNr>
    class GptChannel : public IPeriodicChannelEx, public IOneShotChannelEx
    {
     public:
        inline GptChannel();
        inline virtual ~GptChannel() override;

        // periodic interface
        inline errorCode begin(callback_t cb, float period, bool startImmediately = true) override;
        inline errorCode setPeriod(float) override;
        inline errorCode start() override;

        // oneShot interface
        inline errorCode begin(callback_t cb) override;
        inline errorCode trigger(float period) override;
        inline errorCode triggerDirect(uint32_t reload) override;
        inline errorCode getTriggerReload(float delay, uint32_t *reload) override;

        // common interface
        inline errorCode stop() override;
       // inline errorCode end() override;
        inline float getMaxPeriod() override;

        static bool isInitialized;

     protected:
        static void isr();

        static uint32_t reload;
        static callback_t callback;
        static bool isPeriodic;

        static constexpr IRQ_NUMBER_t irq  = channelNr == 0 ? IRQ_GPT1 : IRQ_GPT2;
        static constexpr IMXRT_GPT_t *regs = channelNr == 0 ? (IMXRT_GPT_t *)IMXRT_GPT1_ADDRESS : (IMXRT_GPT_t *)IMXRT_GPT2_ADDRESS;

        static_assert(channelNr < 2, "GPT channel number must be < 2");
    };

    // INLINE IMPLEMENTATION ================================================================

    template <unsigned chnr>
    GptChannel<chnr>::GptChannel()
    {
        if (chnr == 0)                                                                         // GPT1 clock settings
            CCM_CCGR1 |= CCM_CCGR1_GPT1_BUS(CCM_CCGR_ON) | CCM_CCGR1_GPT1_SERIAL(CCM_CCGR_ON); //
        else                                                                                   // GPT2
            CCM_CCGR0 |= CCM_CCGR0_GPT2_BUS(CCM_CCGR_ON) | CCM_CCGR0_GPT2_SERIAL(CCM_CCGR_ON); //
                                                                                               //
        if (USE_GPT_PIT_150MHz)                                                                // timer clock setting from config.h
            CCM_CSCMR1 &= ~CCM_CSCMR1_PERCLK_CLK_SEL;                                          // use F_BUS
        else                                                                                   //
            CCM_CSCMR1 |= CCM_CSCMR1_PERCLK_CLK_SEL;                                           // use 24MHz
                                                                                               //
        regs->CR = GPT_CR_CLKSRC(0x001) | GPT_CR_ENMOD;                                        // stopped, restart mode and peripheral clock source

        attachInterruptVector(irq, isr);
        NVIC_ENABLE_IRQ(irq);
    }

    template <unsigned chnr>
    void GptChannel<chnr>::isr()
    {
        if (!isPeriodic)
            regs->CR &= ~GPT_CR_EN; // stop timer in one shot mode

        regs->SR = 0x3F;     // reset all interrupt flags
        callback();          // we only enabled the OF1 interrupt-> no need to find out which interrupt was actually called
        asm volatile("dsb"); // wait until register changes propagated through the cache
    }

    // periodic
    template <unsigned chnr>
    errorCode GptChannel<chnr>::begin(callback_t cb, float period, bool startImmediately)
    {
        if (cb == nullptr || period < 0.1) return errorCode::arguments;

        callback   = cb;
        reload     = GptHelpers::microsecondToCycles(period);
        regs->OCR1 = reload;
        isPeriodic = true;

        return startImmediately ? start() : errorCode::OK;
    }

    // one shot
    template <unsigned chnr>
    errorCode GptChannel<chnr>::begin(callback_t cb)
    {
        if (cb == nullptr) return errorCode::arguments;

        callback   = cb;
        isPeriodic = false;

        return errorCode::OK;
    }

    template <unsigned chnr>
    errorCode GptChannel<chnr>::triggerDirect(uint32_t reload)
    {
        regs->SR   = 0x3F;         // clear all interupt flags
        regs->IR   = GPT_IR_OF1IE; // enable OF1 interrupt
        regs->OCR1 = reload;       // set overflow value
        regs->CR |= GPT_CR_EN;     // enable timer

        return errorCode::OK;
    }

    template <unsigned chnr>
    errorCode GptChannel<chnr>::trigger(float delay)
    {
        return triggerDirect(GptHelpers::microsecondToCycles(delay));
    }

    template <unsigned chnr>
    errorCode GptChannel<chnr>::getTriggerReload(float delay, uint32_t *reload)
    {
        *reload = GptHelpers::microsecondToCycles(delay);
        return errorCode::OK;
    }

    template <unsigned chnr>
    GptChannel<chnr>::~GptChannel()
    {
        stop();
        callback = nullptr;
        GPTFactory::destroyTimer(this);
    }

    template <unsigned chnr>
    errorCode GptChannel<chnr>::setPeriod(float)
    {
        return errorCode::notImplemented;
    }

    template <unsigned chnr>
    errorCode GptChannel<chnr>::start()
    {
        regs->SR = 0x3F;         // clear all interupt flags
        regs->IR = GPT_IR_OF1IE; // enable OF1 interrupt
        regs->CR |= GPT_CR_EN;   // enable timer
        return errorCode::OK;
    }

    template <unsigned chnr>
    errorCode GptChannel<chnr>::stop()
    {
        regs->CR &= ~GPT_CR_EN; // disable timer
        regs->IR = 0;
        return errorCode::OK;
    }

    // template <unsigned chnr>
    // errorCode GptChannel<chnr>::end()
    // {
    //     // stop();
    //     // callback = nullptr;
    //     // return GPTFactory::destroyTimer(nullptr);
    //     return errorCode::notImplemented;
    // }

    template <unsigned chnr>
    float GptChannel<chnr>::getMaxPeriod()
    {
        return GptHelpers::getMaxMicros() / 1E6;
    }

    // initialization of static variables

    template <unsigned chnr>
    callback_t GptChannel<chnr>::callback = nullptr;

    template <unsigned chnr>
    uint32_t GptChannel<chnr>::reload = 0;

    template <unsigned chnr>
    bool GptChannel<chnr>::isPeriodic = true;

    template <unsigned chnr>
    bool GptChannel<chnr>::isInitialized = false;

} // namespace TeensyTimerTool