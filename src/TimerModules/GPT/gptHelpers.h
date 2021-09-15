#pragma once

#include "ErrorHandling/error_codes.h"
#include "gptMap.h"

namespace TeensyTimerTool
{
    namespace GptHelpers
    {
        extern float getMaxMicros();
        extern float getMaxPeriod();
        extern uint32_t microsecondToCycles(float micros);
        extern errorCode removeTimer(IMXRT_GPT_t *regs);

    }; // namespace GptChannel
} // namespace TeensyTimerTool