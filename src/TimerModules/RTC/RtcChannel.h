#pragma once

#include "../../ITimerChannel.h"
#include "Arduino.h"
#include "ErrorHandling/error_codes.h"

namespace TeensyTimerTool
{
    class RtcChannel : public ITimerChannel
    {
     public:
        inline RtcChannel(callback_t *callbackSlot);
        inline ~RtcChannel();

        inline errorCode begin(callback_t cb, float period, bool periodic) override;
        inline errorCode start() override;
        inline errorCode stop() override;
        inline errorCode setPeriod(float us) override;
        float getMaxPeriod() const override { return 1.0; } // seconds

        void release() override // channel is a static object in RTC.cpp, stop it instead of delete
        {
            stop();
            *pCallback = nullptr;
            allocated  = false;
        }

        bool allocated = false; // RTC has only one channel, see RTC_t::getTimer()

     protected:
        inline errorCode setPeriodNoStart(float us);
        float period;
    };

    // IMPLEMENTATION ==============================================

    RtcChannel::RtcChannel(callback_t *callbackSlot)
    {
        pCallback = callbackSlot;
    }
    RtcChannel::~RtcChannel()
    {
        stop();
        pCallback = nullptr;
    }

    errorCode RtcChannel::begin(callback_t cb, float period, bool periodic)
    {
        if (!periodic) return errorCode::notImplemented;

        stop();
        *pCallback = cb;
        return setPeriodNoStart(period);
    }

    errorCode RtcChannel::setPeriodNoStart(float us)
    {
        errorCode status = us > 1E6f ? errorCode::periodOverflow : errorCode::OK;
        period           = constrain(us, 1E6f / 32768.0f, 1E6f);
        return status;
    }

    errorCode RtcChannel::start()
    {
        // periodic interrupt with 2^val RTC ticks: next power of 2 >= requested ticks, i.e. the next longer period
        uint32_t ticks = (uint32_t)ceilf(period * (32768 / 1E6f) - 1E-3f); // tolerance for float rounding of exact powers
        unsigned val   = ticks <= 1 ? 0 : 32 - __builtin_clz(ticks - 1);
        SNVS_HPCR = SNVS_HPCR_PI_FREQ(val);

        SNVS_HPCR |= SNVS_HPCR_PI_EN; // enable periodic interrupt
        while (!(SNVS_HPCR & SNVS_HPCR_PI_EN)) {}
        return errorCode::OK;
    }

    errorCode RtcChannel::stop()
    {
        SNVS_HPCR &= ~SNVS_HPCR_PI_EN;
        while ((SNVS_HPCR & SNVS_HPCR_PI_EN)) {}
        return errorCode::OK;
    }

    errorCode RtcChannel::setPeriod(float _us)
    {
        stop();
        errorCode status = setPeriodNoStart(_us);
        start();
        return status;
    }

} // namespace TeensyTimerTool
