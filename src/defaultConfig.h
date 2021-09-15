#pragma once

#include "boardDef.h"
namespace TeensyTimerTool
{
    //--------------------------------------------------------------------------------------------
    // Timer pool defintion
    // Add, and sort and remove to define the timer pool. Timers will be allocted from left to right

#if defined(ARDUINO_TEENSY_MICROMOD)
    //TimerGenerator *const timerPool[] = {GPT1, GPT2, TMR1, TMR2, TMR3, TMR4, TCK};

#elif defined(ARDUINO_TEENSY40)
    //TimerGenerator *const timerPool[] = {GPT1, GPT2, TMR1, TMR2, TMR3, TMR4, TCK};

#elif defined(ARDUINO_TEENSY41)
    //TimerGenerator *const timerPool[] = {}; //GPT1, GPT2, TMR1, TMR2, TMR3, TMR4, TCK};
#endif

    //--------------------------------------------------------------------------------------------
    // Default settings for various timers

    // TMR (QUAD)
    constexpr int TMR_DEFAULT_PSC = PSC_1; // Allowed prescaling values: PSC_1, PSC_2, PSC_4 ... PSC_128, clock = 150MHz

    // FTM
    constexpr int FTM_DEFAULT_PSC[] = // Allowed prescaling values: PSC_AUTO, PSC_1, PSC_2, PSC_4 ... PSC_128, clock = FBUS
        {                             // (PSC_AUTO adjusts prescaler to get roughly 2 timer ticks per µs)
            /*FTM0*/ PSC_AUTO,
            /*FTM1*/ PSC_AUTO,
            /*FTM2*/ PSC_AUTO,
            /*FTM3*/ PSC_AUTO};


    // GPT & PID
    constexpr bool USE_GPT_PIT_150MHz = false; // changes the clock source for GPT and PIT from 24MHz (standard) to 150MHz, might have side effects!

    // TCK
    constexpr unsigned NR_OF_TCK_TIMERS = 20; // How many TCK timers shall be available

    //-------------------------------------------------------------------------------------------
    // Yield Strategy
    // YIELD_NONE:      lib doesn't touch yield. Make sure to call TeensyTimerTool::tick as often as possible
    // YIELD_STANDARD:  uses the standard yield function and adds a call to TeensyTimerTool::tick(). Lots of overhead in yield...
    // YIELD_OPTIMIZED: generate an optimized yield which only calls TeensyTimerTool::Tick()  (recommended if you don't use SerialEvents)

#define YIELD_TYPE YIELD_STANDARD

    //--------------------------------------------------------------------------------------------
    // Callback type
    // Uncomment if you prefer function pointer callbacks instead of std::function callbacks

    //   #define PLAIN_VANILLA_CALLBACKS

    //--------------------------------------------------------------------------------------------
    // Additionally use c++14 user literals (e.g. 3.4s, 50ms 2.4_kHz...) for time inputs
    // Comment the following line if you don't want this.

#define USE_TIME_LITERALS

    //--------------------------------------------------------------------------------------------
    // Advanced Features
    // Uncomment if you need access to advanced features

    //   #define ENABLE_ADVANCED_FEATURES
} // namespace TeensyTimerTool