#pragma once
#include "Arduino.h"
#include "ErrorHandling/error_codes.h"
#include "config.h"

// we can not use staticFunctional with too old compilers
#if not defined(PLAIN_VANILLA_CALLBACKS)
    #if (__GNUC__ < 7)
        #warning "can't use staticFunctional with GCC < V7! Fallback to function pointer callbacks"
    #else
        #define USE_MODERN_CALLBACKS
        #include "staticFunctional.h"
    #endif
#endif
namespace TeensyTimerTool
{
    #if defined(USE_MODERN_CALLBACKS)
        using staticFunctional::function;
        using callback_t  = function<void(void)>;
        using errorFunc_t = function<void(errorCode)>;

        extern void attachErrFunc(errorFunc_t);
        extern errorCode postError(errorCode);
    #else
        using callback_t  = void (*)();
        using errorFunc_t = void (*)(errorCode);

        extern void attachErrFunc(errorFunc_t);
        extern errorCode postError(errorCode);
    #endif

    #if defined(TTT_TEENSY4X)
    #define TTT_F_CPU F_CPU_ACTUAL
    #else
    #define TTT_F_CPU F_CPU
    #endif
} // namespace TeensyTimerTool