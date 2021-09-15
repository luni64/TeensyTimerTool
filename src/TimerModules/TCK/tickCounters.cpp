
#include "tickCounters.h"
#include "../boardDef.h"
#include "Arduino.h"

#if defined(TEENSY4X)
#define F_CPU_TTT F_CPU_ACTUAL
#else
#define F_CPU_TTT F_CPU
#endif

namespace TeensyTimerTool
{
    CycleCounterBase::CycleCounterBase()
    {
        if (ARM_DWT_CYCCNT == ARM_DWT_CYCCNT)
        {
            ARM_DEMCR |= ARM_DEMCR_TRCENA;
            ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA;
        }
    }

    //---------------------------------------------------------------------
    // CycleCounter32

    uint32_t CycleCounter32::getCount()
    {
        return ARM_DWT_CYCCNT;
    }

    uint32_t CycleCounter32::us2ticks(float us)
    {
        return us * (F_CPU_TTT / 1E6f);
    }

    float CycleCounter32::getMaxMicros()
    {
        return 0;
    }

    //---------------------------------------------------------------------
    // CycleCounter64

    uint64_t CycleCounter64::getCount()
    {
        static uint32_t lastCyCnt = ARM_DWT_CYCCNT;
        static uint32_t curHIGH   = 0;

        uint32_t now = ARM_DWT_CYCCNT;
        if (now < lastCyCnt)
        {
            curHIGH++;
        }
        lastCyCnt = now;
        return ((uint64_t)curHIGH << 32) | now;
    }

    uint64_t CycleCounter64::us2ticks(float us)
    {
        return us * (F_CPU_TTT / 1E6f);
    }

    float CycleCounter64::getMaxMicros()
    {
        return 0;
    }

    //------------------------------------------------------------------
    // MicrosCounter

    uint32_t MicrosCounter::getCount()
    {
        return micros();
    }

    uint32_t MicrosCounter::us2ticks(float us)
    {
        return us;
    }

    float MicrosCounter::getMaxMicros()
    {
        return 0;
    }

#if defined(TEENSY4X)
    //------------------------------------------------------------------
    // RtcCounter

    uint64_t RtcCounter::getCount()
    {
        uint32_t hi1 = SNVS_HPRTCMR, lo1 = SNVS_HPRTCLR;
        while (true)
        {
            uint32_t hi2 = SNVS_HPRTCMR, lo2 = SNVS_HPRTCLR;
            if (lo1 == lo2 && hi1 == hi2)
            {
                return (uint64_t)hi2 << 32 | lo2;
            }
            hi1 = hi2;
            lo1 = lo2;
        }
    }

    uint64_t RtcCounter::us2ticks(float us)
    {
        return us * (32'768 / 1E6);
    }

    float RtcCounter::getMaxMicros()
    {
        return 0;
    }

#endif

} // namespace TeensyTimerTool