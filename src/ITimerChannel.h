#pragma once

#include "types.h"

namespace TeensyTimerTool
{
    class ITimerChannel
    {
     public:
        virtual void begin(callback_t callback, unsigned period, bool oneShot) = 0;

        virtual void setPeriod(uint32_t microSeconds);
        virtual uint32_t getPeriod() { return 0; }

        virtual void trigger(uint32_t delay) = 0;
        virtual void start(){};
        virtual errorCode stop() { return errorCode::OK; }
        inline void setCallback(callback_t);

     protected:
        inline ITimerChannel(callback_t* cbStorage = nullptr);
        callback_t* pCallback;
    };

    // IMPLEMENTATION ====================================================

    ITimerChannel::ITimerChannel(callback_t* cbStorage)
    {
        this->pCallback = cbStorage;
    }

    void ITimerChannel::setCallback(callback_t cb)
    {
        *pCallback = cb;
    }

} // namespace TeensyTimerTool