#ifdef ESP32

#include "../../TeensyTimerTool.h"

#if defined(HAS_TCK) and defined(ESP32)

namespace TeensyTimerTool
{
    bool TCK_t::isInitialized = false;
    TckChannel* TCK_t::channels[maxTckChannels];
}

void yield() { TeensyTimerTool::TCK_t::tick(); }

#endif

#endif