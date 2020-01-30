#include "../../boardDef.h"

#if defined(HAS_TCK)

#include "TCK.h"
namespace TeensyTimerTool 
{
    bool TCK_t::isInitialized = false;
    TckChannel* TCK_t::channels[maxTckChannels];
}

void yield() { TeensyTimerTool::TCK_t::tick(); }

#endif