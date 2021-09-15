#include "../../config.h"

#if defined(TEENSYDUINO)

    #include "TCK.h"

    namespace TeensyTimerTool
    {
        bool TCK_t::isInitialized = false;
        ITimerChannelEx* TCK_t::channels[NR_OF_TCK_TIMERS];

        void removeTimer(ITimerChannelEx* channel) // need this as a free function in a cpp file to avoid circle includes from tckPeriodicChannel / tckOneShotChannel
        {
            TCK_t::removeTimer(channel);
        }
    }




//----------------------------------------------------------------------
    #if YIELD_TYPE == YIELD_OPTIMIZED

    void yield()
    {
        TeensyTimerTool::TCK_t::tick();
    }

//----------------------------------------------------------------------
    #elif YIELD_TYPE == YIELD_STANDARD

    #include "EventResponder.h"

    namespace TeensyTimerTool
    {
        static EventResponder er;

        void initYieldHook()
        {
            er.attach([](EventResponderRef r)
            {
                TeensyTimerTool::TCK_t::tick();
                r.triggerEvent();
            });
            er.triggerEvent();
        }
    }
    #endif
#endif
