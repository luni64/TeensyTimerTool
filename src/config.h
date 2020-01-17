#pragma once
#include "boardDef.h"


namespace TeensyTimerTool
{
     struct ITimerChannel;     
     using TimerGenerator = ITimerChannel *();

// General settings ---------------------------------------------------------------------------------

// #define PLAIN_CALLBAC   // un-comment if you prefer a plain void(*) function pointer for callbacks


/** Teensy 4.0 ***********************************************************************************
    *
    *  Available Timer Modules:
    *  - TMR1-TMR4:  Quad Timer, each module has 4 16-bit timer channels
    *  - GPT1-GPT2:  General purpose timer, one 32-bit channel per timer
    *  - TCK:        Tick-Timer, 10 32-bit channels  */

#if defined(T4_0)
constexpr TimerGenerator *timerPool[] = {GPT1, GPT2, TMR1, TMR2, TMR3, TMR4, TCK};

/** Teensy 3.6 ***************************************************************************************
    *
    *  Available Timer Modueles:
    *  - FTM 1: 8 16-bit channels
    *  - FTM 2: 8 16-bit channels
    *
    ****************************************************************************************************/
#elif defined(T3_6)
constexpr TimerGenerator *timerPool[] = {TMR1, TMR2, TMR3, TMR4};

/** Teensy 3.5 **************************************************************************************
    *
    *  Available Timer Modueles:
    *  - 
    *  -
    * 
    ****************************************************************************************************/
#elif defined(T3_5)
constexpr TimerGenerator *timerPool[] = {TCK};

/** Teensy 3.2 ****************************************************************************************
    * 
    *  Available Timer Modueles:
    *  - 
    *  -
   * ****************************************************************************************************/
#elif defined(T3_2)
constexpr TimerGenerator *timerPool[] = {TCK};

/** ESP32 **************************************************************************************
    *
    *  Available Timer Modueles:
    *  - 
    *  -
    * 
    ****************************************************************************************************/
#elif defined(ESP32)
constexpr TimerGenerator *timerPool[] = {TCK};

/** UNO **************************************************************************************
    *
    *  Available Timer Modueles:
    *  - 
    *  -
    * 
    ****************************************************************************************************/
#elif defined(UNO)
constexpr TimerGenerator *timerPool[] = {TCK};
#endif

//---------
constexpr unsigned timerCnt = sizeof(timerPool) / sizeof(timerPool[0]);

}


#include "types.h"

