#include "boardDef.h"

#if defined(TEENSY4X)

#include "gptHelpers.h"
#include "gpt.h"

namespace TeensyTimerTool
{
    namespace GptHelpers
    {
        namespace // private
        {
            float clock()
            {
                return (CCM_CSCMR1 & CCM_CSCMR1_PERCLK_CLK_SEL) ? 24.0f : (F_BUS_ACTUAL / 1000000.0f);
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

        errorCode removeTimer(IMXRT_GPT_t *regs)
        {
            if (regs == (IMXRT_GPT_t *)&IMXRT_GPT1)
                GPT_t<0>::removeTimer();

            else if (regs == (IMXRT_GPT_t *)&IMXRT_GPT2)
                GPT_t<1>::removeTimer();
            else
                postError(errorCode::GTP_err);

            return errorCode::OK;
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

    } // namespace GptChannel

} // namespace TeensyTimerTool

#endif