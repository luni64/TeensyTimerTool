#pragma once

#include "types.h"

namespace TeensyTimerTool
{
    class ITimerChannel
    {
     public:
        virtual errorCode begin(callback_t callback, uint32_t period, bool oneShot) = 0;
        virtual errorCode begin(callback_t callback, float period, bool oneShot) { return postError(errorCode::wrongType); };
        virtual errorCode trigger(uint32_t delay) = 0;
        virtual errorCode trigger(float delay) { return postError(errorCode::wrongType); }

        virtual float getMaxPeriod(){ postError(errorCode::notImplemented); return 0;};

        virtual void setPeriod(uint32_t microSeconds);
        virtual uint32_t getPeriod() { return 0; }

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