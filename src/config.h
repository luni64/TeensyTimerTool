#pragma once


#if defined(__MKL26Z64__) && defined(TEENSYDUINO)
#elif defined(__MK20DX128__) && defined(TEENSYDUINO)
#elif defined(__MK20DX256__) && defined(TEENSYDUINO)
#elif defined(__MK64FX512__) && defined(TEENSYDUINO)

#elif defined(__MK66FX1M0__) && defined(TEENSYDUINO)
#    define T3_6

#elif defined(__IMXRT1062__) && defined(TEENSYDUINO)
#define T4_0
#include "Teensy/TMR/TMR.h"
#include "Teensy/GPT/GPT.h"
#include "Teensy/TCK/TCK.h"

#elif defined(ESP32)
#include "ESP32/TCK/TCK.h"

#else
#    error "Board not supported"
#endif

namespace TeensyTimerTool
{    
    // General settings ---------------------------------------------------------------------------------

    // #define PLAIN_CALLBAC   // un-comment if you prefer a plain void(*) function pointer for callbacks


   /** Teensy 4.0 ***********************************************************************************
    *
    *  Available Timer Modules:
    *  - TMR1-TMR4:  Quad Timer, each module has 4 16-bit timer channels
    *  - GPT1-GPT2:  General purpose timer, one 32-bit channel per timer
    *  - TCK:        Tick-Timer, 10 32-bit channels  */
   
    #if defined(T4_0)  
    constexpr TimerGenerator* allowedTimers[] = {GPT1, GPT2, TMR1, TMR2, TMR3, TMR4, TCK};

   
   /** Teensy 3.6 ***************************************************************************************
    *
    *  Available Timer Modueles:
    *  - FTM 1: 8 16-bit channels
    *  - FTM 2: 8 16-bit channels
    *
    ****************************************************************************************************/
    #elif defined(T3_6)
    constexpr TimerGenerator* allowedTimers[] = {TMR1, TMR2, TMR3, TMR4};

    
   /** Teensy 3.5 **************************************************************************************
    *
    *  Available Timer Modueles:
    *  - 
    *  -
    * 
    ****************************************************************************************************/
     #elif defined(T3_5)
    constexpr TimerGenerator* allowedTimers[] = {TCK};

    /** Teensy 3.2 ****************************************************************************************
    * 
    *  Available Timer Modueles:
    *  - 
    *  -
   * ****************************************************************************************************/   
    #elif defined(T3_2)
    constexpr TimerGenerator* allowedTimers[] = {TCK};

    /** ESP32 **************************************************************************************
    *
    *  Available Timer Modueles:
    *  - 
    *  -
    * 
    ****************************************************************************************************/
    #elif defined(ESP32)
    constexpr TimerGenerator* allowedTimers[] = {TCK};



    /** UNO **************************************************************************************
    *
    *  Available Timer Modueles:
    *  - 
    *  -
    * 
    ****************************************************************************************************/
    #elif defined(UNO)
    constexpr TimerGenerator* allowedTimers[] = {TCK};
    #endif

//---------
constexpr unsigned timerCnt = sizeof(allowedTimers) / sizeof(allowedTimers[0]);

}
