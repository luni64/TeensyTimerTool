#include "timerFactory.h"
#include "boardDef.h"

#if defined(TEENSY4X)
#include "TimerModules/GPT/gptFactory.h"
#include "TimerModules/IOneShotChannel.h"
#include "TimerModules/IPeriodicChannel.h"
#include "TimerModules/TMR/tmr.h"

#elif defined(TEENSY3X)

#endif

#include "../TCK/tck.h"
#include "../TCK/tickCounters.h"

namespace TeensyTimerTool
{
    namespace TimerFactory
    {
        ITimerChannelEx *makeTimer(TimerModule module, TimerType timerType)
        {
            switch (module)
            {
                // Software timers, 32 and 64 bit wide
                case TimerModule::TCK:
                case TimerModule::TCK32:
#if !defined(ARDUINO_TEENSYLC)
                    return TCK_t::makeTimer<CycleCounter32>(timerType);
#else
                    return TCK_t::makeTimer<MicrosCounter>(timerType);
#endif
#if defined(TEENSY3X)
                case TimerModule::TCK64: return TCK_t::makeTimer<CycleCounter64>(timerType);
#endif

#if defined(TEENSY4X)

                // TMR (aka QUAD) timers 16bit, fouur channels per module --------------------------
                case TimerModule::TMR:
                    if (ITimerChannelEx *t = TMR_t<3>::makeTimer(timerType)) { return t; }
                    if (ITimerChannelEx *t = TMR_t<2>::makeTimer(timerType)) { return t; }
                    if (ITimerChannelEx *t = TMR_t<1>::makeTimer(timerType)) { return t; }
                    if (ITimerChannelEx *t = TMR_t<0>::makeTimer(timerType)) { return t; }
                    return nullptr;

                case TimerModule::TMR1: return TMR_t<0>::makeTimer(timerType);
                case TimerModule::TMR2: return TMR_t<1>::makeTimer(timerType);
                case TimerModule::TMR3: return TMR_t<2>::makeTimer(timerType);
                case TimerModule::TMR4: return TMR_t<3>::makeTimer(timerType);

                // General Purpose Timers 32bit, one channel per module ------------------------
                case TimerModule::GPT:
                case TimerModule::GPT1:
                case TimerModule::GPT2:
                    return GPTFactory::makeTimer(module, timerType);

                case TimerModule::PIT: return nullptr;

                // 64 bit software timers
                case TimerModule::TCK64: return TCK_t::makeTimer<CycleCounter64>(timerType);
                // Real Time Clock based software timer 64bit, one channel -------------------------
                case TimerModule::RTC: return TCK_t::makeTimer<RtcCounter>(timerType);

#endif
                default:
                    Serial.println("Factory, timer not available");
                    return nullptr;
            }
        }

        // ITimerChannelEx *makeGPT(TimerModule module, TimerType timerType)
        // {
        //     void *timer;

        //     switch (module)
        //     {
        //         case TimerModule::GPT1:
        //             timer = new (std::nothrow) GptPeriodicChannel2<0>();
        //             break;

        //         case TimerModule::GPT2:
        //             timer = new (std::nothrow) GptPeriodicChannel2<1>();
        //             break;

        //         case TimerModule::GPT:
        //             if (!GptPeriodicChannel2<0>::isInitialized)
        //                 timer = new (std::nothrow) GptPeriodicChannel2<0>();
        //             else if (!GptPeriodicChannel2<1>::isInitialized)
        //                 timer = new (std::nothrow) GptPeriodicChannel2<1>();
        //             else
        //                 timer = nullptr;
        //             break;

        //         default:
        //             timer = nullptr;
        //             break;
        //     }

        //     return (ITimerChannelEx *)timer;

        //     // switch (timerType)
        //     // {
        //     //     case TimerType::periodic
        //     //     return (IPeriodicChannelEx)

        //     //         default:
        //     //         break;
        //     // }

        // return timerType == TimerType::periodic ? ()
        //}

    } // namespace TimerFactory
} // namespace TeensyTimerTool
