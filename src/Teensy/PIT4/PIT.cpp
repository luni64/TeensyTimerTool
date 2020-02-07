#include "../../boardDef.h"

#if defined(T4_0)

#include "PIT.h"

namespace TeensyTimerTool
{
    bool PIT_t::isInitialized = false;
    callback_t PIT_t::callback = nullptr;
    PITChannel* PIT_t::channel = nullptr;    
    callback_t PIT_t::callbacks[4];
}

#endif