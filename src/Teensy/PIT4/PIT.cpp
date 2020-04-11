#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)

#include "PIT.h"

namespace TeensyTimerTool
{
    bool PIT_t::isInitialized = false;
    PITChannel PIT_t::channel[4] = {{0}, {1}, {2}, {3}};

     uint32_t PITChannel::clockFactor = 1;
}

#endif