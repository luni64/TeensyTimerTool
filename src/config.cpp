#include "config.h"
#include "boardDef.h"

#if defined(T4_0)
    #include "Teensy/TMR/TMR.h"
    #include "Teensy/GPT/GPT.h"
    #include "Teensy/TCK/TCK.h"

    namespace TeensyTimerTool
    {
        TimerGenerator* const TMR1 = TMR_t<0>::getTimer;
        TimerGenerator* const TMR2 = TMR_t<1>::getTimer;
        TimerGenerator* const TMR3 = TMR_t<2>::getTimer;
        TimerGenerator* const TMR4 = TMR_t<3>::getTimer;

        TimerGenerator* const GPT1 = GPT_t<0>::getTimer;
        TimerGenerator* const GPT2 = GPT_t<1>::getTimer;

        TimerGenerator* const TCK = TCK_t::getTimer;
    }

#elif defined (T3_6) || defined (T3_5) 
    #include "Teensy/FTM/FTM.h"
    #include "Teensy/TCK/TCK.h"

    namespace TeensyTimerTool
    {
        TimerGenerator* const TCK = TCK_t::getTimer;

        TimerGenerator* const FTM0 = FTM_t<0>::getTimer;
        TimerGenerator* const FTM1 = FTM_t<1>::getTimer;
        TimerGenerator* const FTM2 = FTM_t<2>::getTimer;
        TimerGenerator* const FTM3 = FTM_t<3>::getTimer;
        TimerGenerator* const FTM4 = FTM_t<3>::getTimer;

    }
#elif defined(T3_2) 
    #include "Teensy/FTM/FTM.h"
    #include "Teensy/TCK/TCK.h"

    namespace TeensyTimerTool
    {
        TimerGenerator* const TCK = TCK_t::getTimer;

        TimerGenerator* const FTM0 = FTM_t<0>::getTimer;
        TimerGenerator* const FTM1 = FTM_t<1>::getTimer;
        TimerGenerator* const FTM2 = FTM_t<2>::getTimer;
    }

#elif defined(T3_0)
    #include "Teensy/FTM/FTM.h"
    #include "Teensy/TCK/TCK.h"

    namespace TeensyTimerTool
    {
        TimerGenerator* const TCK = TCK_t::getTimer;

        TimerGenerator* const FTM0 = FTM_t<0>::getTimer;
        TimerGenerator* const FTM1 = FTM_t<1>::getTimer;    
    }

#elif defined(TLC)
    //#include "Teensy/FTM/FTM.h"
    #include "Teensy/TCK/TCK.h"

    namespace TeensyTimerTool
    {
        TimerGenerator* const TCK = TCK_t::getTimer;

        // TimerGenerator* const FTM0 = FTM_t<0>::getTimer;
        // TimerGenerator* const FTM1 = FTM_t<1>::getTimer;
    }

#endif
