#pragma once

#include "boardDef.h"

//================================================
// Board specifics

#if defined(T_LC)

#elif defined(T3_1)

#elif defined (T3_2)    
    #define HAS_TCK 20
    #include "errors.h"
    #include "timer.h"
    #include "kinetis.h"
    #include "Teensy/TCK/TCK.h"

#elif defined (T3_5)

#elif defined (T3_6)    
    #define HAS_FTM

#elif defined(T4_0)    
    #define HAS_TMR 4
    #define HAS_GPT 2
    #define HAS_TCK 10
    #include "errors.h"
    #include "timer.h"
    #include <imxrt.h>
    #include "Teensy/TCK/TCK.h"
    #include "Teensy/GPT/GPT.h"
    #include "Teensy/TMR/TMR.h"

#else
    #error "Board not supported"
#endif

#include "config.h"
