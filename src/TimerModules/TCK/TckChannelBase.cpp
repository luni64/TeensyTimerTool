#if defined(TEENSYDUINO)

#include "TckChannelBase.h"
#include "TCK.h"

#include "Arduino.h"

namespace TeensyTimerTool
{
    TckChannelBase::~TckChannelBase()
    {
        TCK_t::removeTimer(this);
    }
}

#endif