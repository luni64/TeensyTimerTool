#include "config.h"
#include "boardDef.h"

using tick_t = void (*) ();

#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41) || defined(ARDUINO_TEENSY_MICROMOD)
    #include "Teensy/TMR/TMR.h"
    #include "Teensy/GPT/GPT.h"
    #include "Teensy/PIT4/PIT.h"
    #include "Teensy/TCK/TCK.h"

    namespace TeensyTimerTool
    {
        TimerGenerator* const TMR1 = TMR_t<0>::getTimer;
        TimerGenerator* const TMR2 = TMR_t<1>::getTimer;
        TimerGenerator* const TMR3 = TMR_t<2>::getTimer;
        TimerGenerator* const TMR4 = TMR_t<3>::getTimer;

        TimerGenerator* const GPT1 = GPT_t<0>::getTimer;
        TimerGenerator* const GPT2 = GPT_t<1>::getTimer;

        TimerGenerator* const PIT = PIT_t::getTimer;

        TimerGenerator* const TCK = TCK_t::getTimer<uint32_t>;
        TimerGenerator* const TCK32 = TCK_t::getTimer<uint32_t>; // same as TCK
        TimerGenerator* const TCK64 = TCK_t::getTimer<uint64_t>;

        constexpr tick_t tick = &TCK_t::tick;
    }

#elif defined (ARDUINO_TEENSY35) || defined (ARDUINO_TEENSY36)
    #include "Teensy/FTM/FTM.h"
    #include "Teensy/TCK/TCK.h"

    namespace TeensyTimerTool
    {
        TimerGenerator* const TCK = TCK_t::getTimer<uint32_t>;
        TimerGenerator* const TCK32 = TCK_t::getTimer<uint32_t>; // same as TCK
        TimerGenerator* const TCK64 = TCK_t::getTimer<uint64_t>;

        TimerGenerator* const FTM0 = FTM_t<0>::getTimer;
        TimerGenerator* const FTM1 = FTM_t<1>::getTimer;
        TimerGenerator* const FTM2 = FTM_t<2>::getTimer;
        TimerGenerator* const FTM3 = FTM_t<3>::getTimer;
        TimerGenerator* const FTM4 = FTM_t<3>::getTimer;

        constexpr tick_t tick = &TCK_t::tick;
    }

#elif defined(ARDUINO_TEENSY31) || defined (ARDUINO_TEENSY32)
    #include "Teensy/FTM/FTM.h"
    #include "Teensy/TCK/TCK.h"

    namespace TeensyTimerTool
    {
        TimerGenerator* const TCK = TCK_t::getTimer<uint32_t>;
        TimerGenerator* const TCK32 = TCK_t::getTimer<uint32_t>; // same as TCK
        TimerGenerator* const TCK64 = TCK_t::getTimer<uint64_t>;

        TimerGenerator* const FTM0 = FTM_t<0>::getTimer;
        TimerGenerator* const FTM1 = FTM_t<1>::getTimer;
        TimerGenerator* const FTM2 = FTM_t<2>::getTimer;
        constexpr tick_t tick = &TCK_t::tick;
    }

#elif defined(ARDUINO_TEENSY30)
    #include "Teensy/FTM/FTM.h"
    #include "Teensy/TCK/TCK.h"

    namespace TeensyTimerTool
    {
        TimerGenerator* const TCK = TCK_t::getTimer;

        TimerGenerator* const FTM0 = FTM_t<0>::getTimer;
        TimerGenerator* const FTM1 = FTM_t<1>::getTimer;
        constexpr tick_t tick = &TCK_t::tick;
    }

#elif defined(ARDUINO_TEENSYLC)
    #include "Teensy/TCK/TCK.h"

    namespace TeensyTimerTool
    {
        TimerGenerator* const TCK = TCK_t::getTimer<uint32_t>;
        constexpr tick_t tick = &TCK_t::tick;
    }

#endif
