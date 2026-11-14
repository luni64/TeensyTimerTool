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
        double getPeriod() const override { return (1u << piFreq()) / 32768.0; }
        double getResolution() const override { return 1.0 / 32768; }

        void release() override // channel is a static object in RTC.cpp, stop it instead of delete
        {
            stop();
            *pCallback = nullptr;
            allocated  = false;
        }

        bool allocated = false; // RTC has only one channel, see RTC_t::getTimer()

     protected:
        inline errorCode setPeriodNoStart(float us);
        inline unsigned piFreq() const; // periodic interrupt: 2^piFreq() RTC ticks
        float period;
        bool running = false;
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
        if (!periodic) return errorCode::notSupported;

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

    unsigned RtcChannel::piFreq() const
    {
        // periodic interrupt with 2^val RTC ticks: next power of 2 >= requested ticks, i.e. the next longer period
        uint32_t ticks = (uint32_t)ceilf(period * (32768 / 1E6f) - 1E-3f); // tolerance for float rounding of exact powers
        return ticks <= 1 ? 0 : 32 - __builtin_clz(ticks - 1);
    }

    // SNVS_HPCR also holds RTC_EN and the other settings of the real time clock: only change the PI bits
    // (writing the whole register cleared RTC_EN and stopped the clock). PI_FREQ may only change while PI_EN = 0.
    errorCode RtcChannel::start()
    {
        stop();
        uint32_t hpcr = (SNVS_HPCR & ~SNVS_HPCR_PI_FREQ(0xF)) | SNVS_HPCR_PI_FREQ(piFreq());
        SNVS_HPCR     = hpcr;
        SNVS_HPCR     = hpcr | SNVS_HPCR_PI_EN; // enable periodic interrupt
        while (!(SNVS_HPCR & SNVS_HPCR_PI_EN)) {}
        running = true;
        return errorCode::OK;
    }

    errorCode RtcChannel::stop()
    {
        SNVS_HPCR &= ~SNVS_HPCR_PI_EN;
        while ((SNVS_HPCR & SNVS_HPCR_PI_EN)) {}
        running = false;
        return errorCode::OK;
    }

    errorCode RtcChannel::setPeriod(float _us) // a stopped timer stays stopped, start() uses the new period
    {
        errorCode status = setPeriodNoStart(_us);
        if (running) start();
        return status;
    }

} // namespace TeensyTimerTool
