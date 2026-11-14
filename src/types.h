#pragma once
#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "config.h"


#if not defined(PLAIN_VANILLA_CALLBACKS)
        #define USE_MODERN_CALLBACKS
        #include "inplace_function.h"
#endif
namespace TeensyTimerTool
{
    #if defined(USE_MODERN_CALLBACKS)
        /// Callback of the timers: function, lambda expression (captures up to 32 bytes) or functor, signature void()
        using callback_t  = stdext::inplace_function<void(void)>;
        /// Error function for attachErrFunc(): function, lambda expression or functor, signature void(errorCode)
        using errorFunc_t = stdext::inplace_function<void(errorCode)>;

        /// @brief Sets a function that is called for every result other than OK, once per failed call.
        /// @param errorFunction e.g. `ErrorHandler(Serial)` or `[](errorCode e) { ... }`; nullptr removes it
        extern void attachErrFunc(errorFunc_t errorFunction);
        extern errorCode postError(errorCode);
    #else
        /// Callback of the timers (PLAIN_VANILLA_CALLBACKS): plain function or captureless lambda, void()
        using callback_t  = void (*)();
        /// Error function for attachErrFunc() (PLAIN_VANILLA_CALLBACKS): plain function, void(errorCode)
        using errorFunc_t = void (*)(errorCode);

        /// @brief Sets a function that is called for every result other than OK, once per failed call.
        /// @param errorFunction a function void(errorCode); nullptr removes it
        extern void attachErrFunc(errorFunc_t errorFunction);
        extern errorCode postError(errorCode);
    #endif

    #if defined(TTT_TEENSY4X)
    #define TTT_F_CPU F_CPU_ACTUAL
    #else
    #define TTT_F_CPU F_CPU
    #endif
} // namespace TeensyTimerTool
