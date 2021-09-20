#pragma once

#include "Arduino.h"
#include "core_pins.h"
#include "ITimerChannelEx.h"
#include "tckPeriodicChannel.h"
#include "tckOneShotChannel.h"

namespace TeensyTimerTool
{
    extern const unsigned NR_OF_TCK_TIMERS;

    class TCK_t
    {
     public:
        template <typename counterType> static inline ITimerChannel *getTimer();
        template <typename counterType> static inline ITimerChannelEx *makeTimer(TimerType);
        static inline void removeTimer(ITimerChannelEx *);
        static inline void tick();

     protected:
        static bool isInitialized;
        static ITimerChannelEx *channels[NR_OF_TCK_TIMERS];
    };

    // IMPLEMENTATION ==================================================================

    template <typename counterType> ITimerChannelEx *TCK_t::makeTimer(TimerType t)
    {
        Serial.println("tck.h makeTimer");
        if (!isInitialized)
        {
            for (unsigned chNr = 0; chNr < NR_OF_TCK_TIMERS; chNr++)
            {
                channels[chNr] = nullptr;
            }
            isInitialized = true;

            // enable the cycle counter if necessary
            if (ARM_DWT_CYCCNT == ARM_DWT_CYCCNT)
            {
                ARM_DEMCR |= ARM_DEMCR_TRCENA;
                ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;
            }

// initialize the yield hook
#if defined(TEENSYDUINO) && YIELD_TYPE == YIELD_STANDARD
            extern void initYieldHook();
            initYieldHook();
#endif
        }

        for (unsigned chNr = 0; chNr < NR_OF_TCK_TIMERS; chNr++)
        {
            if (channels[chNr] == nullptr)
            {
                switch (t)
                {
                    case TimerType::periodic:
                        channels[chNr] = new(std::nothrow) TckPeriodicChannel<counterType>(nullptr);
                        break;

                    case TimerType::oneShot:
                        channels[chNr] = new(std::nothrow) TckOneShotChannel<counterType>();
                        break;

                    default:
                        break;
                }
                return (ITimerChannelEx*) channels[chNr];
            }
        }
        return nullptr;
    }

    void TCK_t::removeTimer(ITimerChannelEx *channel)
    {
        for (unsigned chNr = 0; chNr < NR_OF_TCK_TIMERS; chNr++)
        {
            if (channels[chNr] == channel)
            {
                channels[chNr] = nullptr;
                break;
            }
        }
    }

    void TCK_t::tick()
    {
        for (unsigned i = 0; i < NR_OF_TCK_TIMERS; i++)
        {
            if (channels[i] != nullptr)
            {
                //Serial.println("ttt");
                channels[i]->tick();
            }
        }
    }
} // namespace TeensyTimerTool
