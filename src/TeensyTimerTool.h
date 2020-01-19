#pragma once

#include "boardDef.h"

//================================================
// Board specifics

#if defined(T_LC)

#elif defined(T3_0)
    #include "kinetis.h"
    #include "timer.h"
    #include "errors.h"
    #include "Teensy/TCK/TCK.h"
    #include "Teensy/FTM/FTM.h"

#elif defined(T3_1) || defined (T3_2)
    #include "kinetis.h"
    #include "timer.h"
    #include "errors.h"
    #include "Teensy/TCK/TCK.h"
    #include "Teensy/FTM/FTM.h"

#elif defined (T3_5)
    #include "kinetis.h"
    #include "timer.h"
    #include "errors.h"
    #include "Teensy/TCK/TCK.h"
    #include "Teensy/FTM/FTM.h"

#elif defined (T3_6)
    #include "kinetis.h"
    #include "timer.h"
    #include "errors.h"
    #include "Teensy/TCK/TCK.h"
    #include "Teensy/FTM/FTM.h"

#elif defined(T4_0)
    #include <imxrt.h>
    #include "timer.h"
    #include "errors.h"
    #include "Teensy/TCK/TCK.h"
    #include "Teensy/GPT/GPT.h"
    #include "Teensy/TMR/TMR.h"

#else
    #error "Board not supported"
#endif

#include "config.h"
