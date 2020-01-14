#pragma once

#include "errors.h"
#include "timer.h"

//================================================
// Board specifics

#if defined(__MKL26Z64__) && defined(TEENSYDUINO)

#elif defined(__MK20DX128__) && defined(TEENSYDUINO)

#elif defined (__MK20DX256__) && defined(TEENSYDUINO)

#elif defined (__MK64FX512__) && defined(TEENSYDUINO)

#elif defined (__MK66FX1M0__) && defined(TEENSYDUINO)
    #define T3_6
    #define HAS_FTM

#elif defined(__IMXRT1062__) && defined(TEENSYDUINO)
    #define T4_0
    #define HAS_TMR 4
    #define HAS_GPT 2
    #define HAS_TCK 10
    #include "Teensy/TCK/TCK.h"
    #include "Teensy/GPT/GPT.h"     
    #include "Teensy/TMR/TMR.h"    
    #include <imxrt.h>

#else
    #error "Board not supported"
#endif


#include "config.h"
