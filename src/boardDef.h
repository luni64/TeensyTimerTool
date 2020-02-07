#pragma once
#include <cstdint>

namespace TeensyTimerTool
{
    class ITimerChannel;
    using TimerGenerator = ITimerChannel*(); //returns a pointer to a free timer channel or nullptr

    // TEENSYDUINO  ==========================================================================
    #if defined(TEENSYDUINO)
    
        #if defined(__MKL26Z64__)
        #define TLC
        #define HAS_TCK 20
        extern TimerGenerator *const TCK;

        #elif defined(__MK20DX128__)
            #define T3_0
            #define HAS_TCK 20
            extern TimerGenerator *const FTM0, * const FTM1;
            extern TimerGenerator *const TCK;

            #elif defined(__MK20DX256__)
            #define T3_2
            #define HAS_TCK 20
            extern TimerGenerator *const FTM0, * const FTM1, * const FTM2;
            extern TimerGenerator *const TCK;

        #elif defined(__MK64FX512__)
            #define T3_5
            #define HAS_TCK 20
            extern TimerGenerator *const FTM0, * const FTM1, * const FTM2, * const FTM3, * const FTM4;
            extern TimerGenerator *const TCK;

        #elif defined(__MK66FX1M0__)
            #define T3_6
            #define HAS_TCK 20
            extern TimerGenerator *const FTM0, *const FTM1, *const FTM2, *const FTM3, *const FTM4;
            extern TimerGenerator *const TCK;

        #elif defined(__IMXRT1062__)
            #define T4_0
            #define HAS_TCK 20
            extern TimerGenerator *const TMR1, *const TMR2, *const TMR3, *const TMR4;
            extern TimerGenerator *const GPT1, *const GPT2;
            extern TimerGenerator *const PIT;
            extern TimerGenerator *const TCK;

        #endif

    // ESP32  ==========================================================================
    #elif defined(ESP32)
        //...

    #else
    #    error "Board not supported"
    #endif
}