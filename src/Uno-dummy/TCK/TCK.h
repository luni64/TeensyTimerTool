#pragma once

#include "TckChannel.h"
#include <Arduino.h>
#include <cstdint>

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
        Serial.printf("TCK getTimer()\n");

        if (!isInitialized)
        {
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
                channels[chNr] = new  TckChannel();
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
        digitalWriteFast(12,HIGH);
        for(unsigned i = 0; i < maxTckChannels; i++)
        {
            if (channels[i] != nullptr )
            {
                channels[i]->tick();
            }
        }
        digitalWriteFast(12,LOW);
    }


    //constexpr TimerGenerator* TCK = TCK_t::getTimer;

} // namespace TeensyTimerTool
