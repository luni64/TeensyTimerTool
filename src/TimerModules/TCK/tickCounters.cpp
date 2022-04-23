
#include "tickCounters.h"
#include "Arduino.h"
#include "types.h"
#include "boardDef.h"

namespace TeensyTimerTool
{
    //---------------------------------------------------------------------
    // CycleCounter32

    uint32_t CycleCounter32::getCount()
    {
        return ARM_DWT_CYCCNT;
    }

    uint32_t CycleCounter32::us2ticks(float us)
    {
        return us * (TTT_F_CPU / 1E6f);
    }

    float CycleCounter32::getMaxMicros()
    {
        return 5E6; //Can't use full 2^32 *1.66ns =7.2s range since we need some headroom to detect reaching target. This works if we call tick() at least once per 2 seconds
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
        return us * (TTT_F_CPU / 1E6f);
    }

    float CycleCounter64::getMaxMicros()
    {
        return 2E16f; //~630 years  can't use full 2^64 *1.66ns range since we need some headroom to detect reaching target.
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
        return 70 * 60 * 1E6; // 70min, can't use full 2^32 µs = 72min range since we need some headroom to detect reaching target. This works if we call tick() at least once per 2min
    }

#if defined(TTT_TEENSY4X)
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
        return us * (32'768 / 1E6f);
    }

    float RtcCounter::getMaxMicros()
    {
        return 2E16f; // ~630 years
    }

#endif

} // namespace TeensyTimerTool