#pragma once

#include "TckChannel.h"
#include "core_pins.h"

namespace TeensyTimerTool
{
    constexpr unsigned maxTckChannels = 20;

    class TCK_t
    {
     public:
        static inline ITimerChannel* getTimer();
        static inline void removeTimer(TckChannel*);
        static inline void tick();

     protected:
        static bool isInitialized;
        static TckChannel* channels[maxTckChannels];
    };

    // IMPLEMENTATION ==================================================================

    ITimerChannel* TCK_t::getTimer()
    {
        if (!isInitialized)
        {
            // enable the cycle counter
            ARM_DEMCR |= ARM_DEMCR_TRCENA;
            ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;

            for (unsigned chNr = 0; chNr < maxTckChannels; chNr++)
            {
                channels[chNr] = nullptr;
            }
            isInitialized = true;
        }

        for (unsigned chNr = 0; chNr < maxTckChannels; chNr++)
        {
            if (channels[chNr] == nullptr)
            {
                channels[chNr] = new TckChannel();
                return channels[chNr];
            }
        }

        return nullptr;
    }

    void TCK_t::removeTimer(TckChannel* channel)
    {
        for (unsigned chNr = 0; chNr < maxTckChannels; chNr++)
        {
            if (channels[chNr] == channel)
            {
                channels[chNr] = nullptr;
                delete channel;
                break;
            }
        }
    }

    void TCK_t::tick()
    {
        for (unsigned i = 0; i < maxTckChannels; i++)
        {
            if (channels[i] != nullptr)
            {
                channels[i]->tick();
            }
        }
    }  
} 
