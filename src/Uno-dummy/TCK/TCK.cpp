//#include "../../TeensyTimerTool.h"
#include "../../config.h"

#if defined(HAS_TCK) and defined(UNO)

namespace TeensyTimerTool
{
    bool TCK_t::isInitialized = false;
    TckChannel* TCK_t::channels[maxTckChannels];
}

void yield() { TeensyTimerTool::TCK_t::tick(); }

#endif
