#pragma once

//#include "TeensyTimerTool.h"
#include "ErrorHandling/ErrorCodes.h"



#if not defined(PLAIN_CALLBACK)
    #include <functional>
    inline void std::__throw_bad_function_call(){while(1){}} // do whatever you want to do instead of an exception
#endif

namespace TeensyTimerTool
{
    #if defined(PLAIN_CALLBACK)
        using callback_t = void (*)();
        using errorFunc_t = void (*)(error);
    #else
        using callback_t = std::function<void(void)>;
        using errorFunc_t = std::function<void(errorCode)>;
    #endif

    struct ITimerChannel;
    using TimerGenerator = ITimerChannel*();
}
