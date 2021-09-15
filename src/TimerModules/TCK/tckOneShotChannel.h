#pragma once

//#include "Arduino.h"
#include "../boardDef.h"
#include "../IOneShotChannelEx.h"
#include "../ErrorHandling/error_codes.h"
#include "TckChannelBase.h"
#include "core_pins.h"
#include "tickCounters.h"

namespace TeensyTimerTool
{
    //class TCK_t;

//#if !defined(ARDUINO_TEENSYLC) // T-LC doesn't have a cycle counter, needs special treatment

    template <typename TickCounter>
    class TckOneShotChannel : public TckChannelBase, public IOneShotChannelEx
    {
        using counter_t = typename TickCounter::counter_t; // use counter type defined in the used TickCounter (uint32_t or uint64_t)

     public:
        TckOneShotChannel(){};
        ~TckOneShotChannel() { end(); }
        errorCode begin(callback_t cb) override;
        errorCode stop() override;
        errorCode end() override;


        errorCode trigger(float delay_us) override;
        errorCode triggerDirect(counter_t reload) override;
        errorCode getTriggerReload(float delay, counter_t *reload) override;

        float getMaxPeriod() override { return TickCounter::getMaxMicros() / 1E6; } // seconds

     protected:
        void tick();
        callback_t callback;

        counter_t startCnt, delayTicks, nextPeriod;
    };

    // IMPLEMENTATION ==============================================

    template <typename TickCounter>
    errorCode TckOneShotChannel<TickCounter>::begin(callback_t cb)
    {
        delayTicks = 0;
        callback   = cb;
        return errorCode::OK;
    }

    // template <typename period_t>
    // errorCode TckOneShotChannel<period_t>::start()
    // {
    //     this->startCnt  = getCycleCounter();
    //     this->triggered = true;
    //     return errorCode::OK;
    // }

    template <typename TickCounter>
    errorCode TckOneShotChannel<TickCounter>::stop()
    {
        delayTicks = 0;
        return errorCode::OK;
    }

    template <typename TickCounter>
    errorCode TckOneShotChannel<TickCounter>::end()
    {
        extern void removeTimer(ITimerChannelEx *channel);
        stop();
        removeTimer(this);
        return errorCode::OK;
    }

    template <typename TickCounter>
    errorCode TckOneShotChannel<TickCounter>::getTriggerReload(float microseconds, counter_t *reload)
    {
        *reload = TickCounter::us2ticks(microseconds);
        return errorCode::OK;
    }

    template <typename TickCounter>
    errorCode TckOneShotChannel<TickCounter>::triggerDirect(counter_t reload)
    {
        delayTicks = reload;
        startCnt   = TickCounter::getCount();
        return errorCode::OK;
    }

    template <typename TickCounter>
    errorCode TckOneShotChannel<TickCounter>::trigger(float microseconds)
    {
        return triggerDirect(TickCounter::us2ticks(microseconds));
    }

    template <typename TickCounter>
    void TckOneShotChannel<TickCounter>::tick()
    {
        if (delayTicks == 0) return;

        static bool lock = false;
        counter_t now    = TickCounter::getCount();
        if (!lock && (now - startCnt) >= delayTicks)
        {
            lock = true;
            callback();
            delayTicks = 0; // one shot only
            lock       = false;
        }
    }

    // template <typename counter_t>
    // counter_t TckOneShotChannel<counter_t>::microsecondToCycles(float microSecond) const
    // {
    //     if (microSecond > getMaxMicros())
    //     {
    //         microSecond = getMaxMicros();
    //         postError(errorCode::periodOverflow);
    //     }
    //     return (counter_t)(microSecond * clock);
    // }

    // SPECIALIZATIONS =================================================================================
    // 32bit Counter -------------------------------------------------------------------------

    // template <>
    // inline float TckOneShotChannel<uint32_t>::getMaxMicros() const
    // {
    //     return 0xF000'0000 / clock; // don't use full range otherwise tick might miss the turnover for large periods
    // }

    // 64bit Counter -------------------------------------------------------------------------

    // template <>
    // inline float TckOneShotChannel<uint64_t>::getMaxMicros() const
    // {
    //     return 0xFFFF'FFFF; // currently limited to 2^32 µs (71.6h). Could be extended to 2^64 but would require change of interface
    // }

    // template <typename ct>
    // uint32_t TckChannel<ct>::CPUCyclesToMicroseond(const uint32_t cpuCycles)
    // {
    //     return (1'000'000.0f / F_CPU) * cpuCycles;
    // }

    // template <typename ct>
    // void TckChannel<ct>::_setNextPeriod(const uint32_t period)
    // {
    //     this->nextPeriod = period;
    // }

    // template <typename ct>
    // errorCode TckChannel<ct>::_setCurrentPeriod(const uint32_t period)
    // {
    //     this->currentPeriod = period;
    //     const bool hasTicked = this->tick();

    //     if (hasTicked)
    //     {
    //         return errorCode::triggeredLate;
    //     } else
    //     {
    //         return errorCode::OK;
    //     }
    // }

    // template <typename ct>
    // errorCode TckChannel<ct>::setPeriod(uint32_t microSeconds)
    // {
    //     const uint32_t period = microsecondToCPUCycles(microSeconds);

    //     this->_setNextPeriod(period);
    //     return this->_setCurrentPeriod(period);
    // }

    // template <typename ct>
    // errorCode TckChannel<ct>::setPeriod(uint32_t microSeconds)
    // {
    //     const uint32_t period = microsecondToCPUCycles(microSeconds);
    //     return this->_setCurrentPeriod(period);
    // }

    // template <typename ct>
    // errorCode TckChannel<ct>::setNextPeriod(uint32_t microSeconds)
    // {
    //     const uint32_t period = microsecondToCPUCycles(microSeconds);
    //     this->_setNextPeriod(period);
    //     return errorCode::OK;
    // }



} // namespace TeensyTimerTool
