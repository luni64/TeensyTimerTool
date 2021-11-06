#if defined(TEENSYDUINO)

#include "TckChannelBase.h"
#include "Arduino.h"
#include "TCK.h"

namespace TeensyTimerTool
{
    TckChannelBase::~TckChannelBase()
    {
        TCK_t::removeTimer(this);
    }
} // namespace TeensyTimerTool

#endif