#include "boardDef.h"

#if defined(TEENSY4X)

#include "core_pins.h"
#include "gptHelpers.h"
#include "types.h"

namespace TeensyTimerTool
{
    namespace GptHelpers
    {
        namespace // private
        {
            float clock()
            {
                return (CCM_CSCMR1 & CCM_CSCMR1_PERCLK_CLK_SEL) ? 24.0f : (F_BUS_ACTUAL / 1'000'000.0f);
            }

        } // namespace

        float getMaxMicros()
        {
            return (float)0xFFFF'FFFE / clock();
        };

        float getMaxPeriod()
        {
            return getMaxMicros() / 1E6;
        }

        uint32_t microsecondToCycles(float micros)
        {
            if (micros > getMaxMicros())
            {
                micros = getMaxPeriod();
                postError(errorCode::periodOverflow);
            }
            return (uint32_t)(clock() * micros) - 1;
        }

    } // namespace GptHelpers
} // namespace TeensyTimerTool
#endif