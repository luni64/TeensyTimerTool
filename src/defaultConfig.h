#pragma once

#include "boardDef.h"
namespace TeensyTimerTool
{
//--------------------------------------------------------------------------------------------
// Timer pool defintion
// Add and sort as required

#if defined(T4_0)
    TimerGenerator* const timerPool[] = {GPT1, GPT2, TMR1, TMR2, TMR3, TMR4, TCK};

#elif defined(T3_6)
    TimerGenerator* const timerPool[] = {FTM0, FTM1, FTM2, FTM3, FTM4, TCK};

#elif defined(T3_5)
    TimerGenerator* const timerPool[] = {FTM0, FTM1, FTM2, FTM3, TCK};

#elif defined(T3_2)
    TimerGenerator* const timerPool[] = {FTM0, FTM1, FTM2, TCK};

#elif defined(T3_0)
    TimerGenerator* const timerPool[] = {FTM0, FTM1, TCK};

#elif defined(TLC)
    TimerGenerator* const timerPool[] = {TCK};

#elif defined(ESP32)
    TimerGenerator* const timerPool[] = {TCK};

#elif defined(UNO)
    TimerGenerator* const timerPool[] = {TCK};
#endif
    constexpr unsigned timerCnt = sizeof(timerPool) / sizeof(timerPool[0]);

//--------------------------------------------------------------------------------------------
// Timer Settings
//
// Default settings for various timers

// TMR (QUAD) Prescaler
    constexpr unsigned TMR_DEFAULT_PSC = 7;     // default prescaler, 0..7 -> prescaler= 1,2,4,...128, timer clock f=150MHz

// FTM Prescaler
    constexpr int FTM_DEFAULT_PSC = -1;         // -1: Auto, 0..7 -> prescaler= 1,2,4,...128, timer clock f=F_BUS

// GPT & PID
    constexpr bool USE_GPT_PIT_150MHz = false;  // changes the clock source for GPT and PIT from 24MHz (standard) to 150MHz, might have side effects!

// TCK Yield Settings
    #define YIELD_NONE       0                  // Doesn't override yield at all, make sure to call TeensyTimerTool::Tick as often as possible
    #define YIELD_STANDARD   1                  // Uses the standard yield function and adds a call to TeensyTimerTool::Tick() lots of overhead in yield...
    #define YIELD_OPTIMIZED  2                  // Generate an optimized yield which only calls TeensyTimerTool::Tick()

    #define YIELD_TYPE YIELD_OPTIMIZED          // Select the required yield strategy here


//--------------------------------------------------------------------------------------------
// Callback type
// Uncomment if you prefer function pointer callbacks instead of std::function callbacks

//    #define PLAIN_VANILLA_CALLBACKS

//--------------------------------------------------------------------------------------------
// Advanced Features
// Uncomment if you need access to advanced features

//#define ENABLE_ADVANCED_FEATURES
}