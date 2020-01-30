#pragma once
#include "ErrorHandling/error_codes.h"
#include "config.h"

#if not defined(PLAIN_VANILLA_CALLBACKS)

    #include <functional>
    inline void std::__throw_bad_function_call()
    {
        while (1) {} // do whatever you want to do instead of an exception
    }

    namespace TeensyTimerTool
    {
        using callback_t = std::function<void(void)>;
        using errorFunc_t = std::function<void(errorCode)>;
        
    }
#else
    namespace TeensyTimerTool
    {
        using callback_t = void (*)();
        using errorFunc_t = void (*)();
        
    }
#endif
