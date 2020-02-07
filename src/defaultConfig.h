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

// TMR (QUAD)
    constexpr unsigned TMR_DEFAULT_PSC = 7;  // default prescaler, 0..7 -> prescaler= 1,2,4,...128, timer clock f=150MHz

// GPT & PID
    constexpr bool USE_GPT_PIT_150MHz = false;// changes the clock source for GPT and PIT from 24MHz (standard) to 150MHz, might have side effects!


//--------------------------------------------------------------------------------------------
// Callback type
// Uncomment if you prefer function pointer callbacks instead of std::function callbacks

//    #define PLAIN_VANILLA_CALLBACKS


//--------------------------------------------------------------------------------------------
// Advanced Features
// Uncomment if you need access to advanced features

//#define ENABLE_ADVANCED_FEATURES
}