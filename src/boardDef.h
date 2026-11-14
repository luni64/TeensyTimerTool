#pragma once
#include <cstdint>

namespace TeensyTimerTool
{
    class ITimerChannel;
    using TimerGenerator = ITimerChannel*(); //returns a pointer to a free timer channel or nullptr

    // Timer types: pass one to the constructor of PeriodicTimer / OneShotTimer, e.g. PeriodicTimer t1(TMR1);
    // Manual: https://luni64.github.io/TeensyTimerTool/reference/timer-types/

    // TEENSYDUINO  ==========================================================================
    #if defined(TEENSYDUINO)
        #if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41) || defined(ARDUINO_TEENSY_MICROMOD)
            #define TTT_TEENSY4X
        #elif defined(ARDUINO_TEENSY30) || defined(ARDUINO_TEENSY31) || defined(ARDUINO_TEENSY32) || defined(ARDUINO_TEENSY35) || defined(ARDUINO_TEENSY36)
            #define TTT_TEENSY3X
        #endif

        #if defined(TTT_TEENSY3X)
            /// Timer type: FlexTimer module FTM0, 16 bit, 8 channels. Using it breaks PWM on the pins of FTM0.
            extern TimerGenerator *const FTM0;
            /// Timer type: FlexTimer module FTM1, 16 bit, 2 channels. Using it breaks PWM on the pins of FTM1.
            extern TimerGenerator *const FTM1;
        #endif
        #if defined(ARDUINO_TEENSY31) || defined(ARDUINO_TEENSY32) || defined(ARDUINO_TEENSY35) || defined(ARDUINO_TEENSY36)
            /// Timer type: FlexTimer module FTM2, 16 bit, 2 channels. Using it breaks PWM on the pins of FTM2.
            extern TimerGenerator *const FTM2;
        #endif
        #if defined(ARDUINO_TEENSY35) || defined(ARDUINO_TEENSY36)
            /// Timer type: FlexTimer module FTM3, 16 bit, 8 channels. Using it breaks PWM on the pins of FTM3.
            extern TimerGenerator *const FTM3;
            /// Timer type: alias of FTM3.
            extern TimerGenerator *const FTM4;
        #endif

        #if defined(TTT_TEENSY4X)
            /// Timer type: quad timer module TMR1, 16 bit, 4 channels, max. 55.9 ms (default prescaler).
            /// Using it breaks PWM on the pins of TMR1 (T4.1: 10, 11, 12).
            extern TimerGenerator *const TMR1;
            /// Timer type: quad timer module TMR2, 16 bit, 4 channels, max. 55.9 ms (default prescaler).
            /// Using it breaks PWM on the pins of TMR2.
            extern TimerGenerator *const TMR2;
            /// Timer type: quad timer module TMR3, 16 bit, 4 channels, max. 55.9 ms (default prescaler).
            /// Using it breaks PWM on the pins of TMR3.
            extern TimerGenerator *const TMR3;
            /// Timer type: quad timer module TMR4, 16 bit, 4 channels, max. 55.9 ms (default prescaler).
            /// Using it breaks PWM on the pins of TMR4.
            extern TimerGenerator *const TMR4;
            /// Timer type: general purpose timer GPT1, 32 bit, 1 channel, max. 179 s (up to 8.5 days with prescaler).
            extern TimerGenerator *const GPT1;
            /// Timer type: general purpose timer GPT2, 32 bit, 1 channel, max. 179 s (up to 8.5 days with prescaler).
            extern TimerGenerator *const GPT2;
            /// Timer type: periodic interrupt timer, 32 bit, 4 channels, max. 179 s. Conflicts with IntervalTimer.
            extern TimerGenerator *const PIT;
            /// Timer type: periodic interrupt of the real time clock, periods of 2^-n s only (1 s … 30.5 µs), periodic only.
            extern TimerGenerator *const RTC;
            /// Timer type: software timer on the RTC counter, resolution 30.5 µs, practically unlimited periods. Callbacks from yield().
            extern TimerGenerator *const TCK_RTC;
        #endif

        #if defined(ARDUINO_TEENSYLC)
            /// Timer type: software timer on micros(), 20 channels (shared by all TCK types). Callbacks from yield().
            extern TimerGenerator *const TCK;
        #else
            /// Timer type: software timer on the 32 bit cycle counter, max. 5 s, 20 channels (shared by all TCK types).
            /// No hardware needed; callbacks from yield().
            extern TimerGenerator *const TCK;
            /// Timer type: same as TCK.
            extern TimerGenerator *const TCK32;
            /// Timer type: software timer on a 64 bit cycle counter, practically unlimited periods. Callbacks from yield().
            extern TimerGenerator *const TCK64;
        #endif

        #if !defined(TTT_TEENSY4X) && !defined(TTT_TEENSY3X) && !defined(ARDUINO_TEENSYLC)
            #error BOARD NOT SUPPORTED
        #endif

        #define YIELD_NONE      0
        #define YIELD_STANDARD  1
        #define YIELD_OPTIMIZED 2

        // Prescaler values for setPrescaler(): divide the timer clock by 2^n
        constexpr int PSC_AUTO = -1;  ///< FTM_DEFAULT_PSC only (configuration): prescaler picked automatically
        constexpr int PSC_1 = 0;      ///< prescaler 1 (TMR, GPT)
        constexpr int PSC_2 = 1;      ///< prescaler 2 (TMR, GPT)
        constexpr int PSC_4 = 2;      ///< prescaler 4 (TMR, GPT)
        constexpr int PSC_8 = 3;      ///< prescaler 8 (TMR, GPT)
        constexpr int PSC_16 = 4;     ///< prescaler 16 (TMR, GPT)
        constexpr int PSC_32 = 5;     ///< prescaler 32 (TMR, GPT)
        constexpr int PSC_64 = 6;     ///< prescaler 64 (TMR, GPT)
        constexpr int PSC_128 = 7;    ///< prescaler 128 (TMR, GPT); TMR default
        constexpr int PSC_256 = 8;    ///< prescaler 256 (GPT only)
        constexpr int PSC_512 = 9;    ///< prescaler 512 (GPT only)
        constexpr int PSC_1024 = 10;  ///< prescaler 1024 (GPT only)
        constexpr int PSC_2048 = 11;  ///< prescaler 2048 (GPT only)
        constexpr int PSC_4096 = 12;  ///< prescaler 4096 (GPT only)

        /// @brief Evaluates the software timers (TCK*) and invokes the callbacks that are due.
        /// @details Called from yield() by default; call it yourself, as often as possible, only with YIELD_NONE.
        extern void(* const tick)();


    #else
    #    error "Board not supported"
    #endif
}
