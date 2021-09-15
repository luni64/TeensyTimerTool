#pragma once

#include <cstdint>

#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41) || defined(ARDUINO_TEENSY_MICROMOD)
#define TEENSY4X
#include "imxrt.h"
// #include "TimerModules/TCK/tck.h"
// #include "TimerModules/TMR/tmr.h"
//#include "TimerModules/GPT/GPT.h"

#elif defined(ARDUINO_TEENSY30) || defined(ARDUINO_TEENSY32) || defined(ARDUINO_TEENSY35) || defined(ARDUINO_TEENSY36)
#define TEENSY3X
#include "kinetis.h"
#elif defined(ARDUINO_TEENSYLC)
#include "kinetis.h"
#else
#error board not defined
#include <stophere>
#endif

namespace TeensyTimerTool
{
    class ITimerChannel;
    using TimerGenerator   = ITimerChannel *(); //returns a pointer to a free timer channel or nullptr
    using TimerGeneratorEx = ITimerChannel *(int);

    enum class TimerType : unsigned {
        periodic,
        oneShot,
        pulseGen
    };

    enum TimerModule : unsigned {
        usePool,
        TCK,
        TCK32,
#if defined(TEENSY4X)
        TCK64,
        RTC,
        PIT,
        TMR,
        TMR1,
        TMR2,
        TMR3,
        TMR4,
        GPT,
        GPT1,
        GPT2,
#elif defined(TEENSY3X)
        TCK64,
        FTM,
        FTM0,
        FTM1,
#elif defined(ARDUINO_TEENSY32)
        FTM2,
#elif defined(ARDUINO_TEENSY35)
        FTM2,
        FTM3,
#elif defined(ARDUINO_TEENSY36)
        FTM2,
        FTM3,
        FTM4,
#endif
    };

#define YIELD_NONE 0
#define YIELD_STANDARD 1
#define YIELD_OPTIMIZED 2

    constexpr int PSC_AUTO = -1;
    constexpr int PSC_1    = 0;
    constexpr int PSC_2    = 1;
    constexpr int PSC_4    = 2;
    constexpr int PSC_8    = 3;
    constexpr int PSC_16   = 4;
    constexpr int PSC_32   = 5;
    constexpr int PSC_64   = 6;
    constexpr int PSC_128  = 7;

    extern void (*const tick)();

} // namespace TeensyTimerTool