#pragma once
#include "Stream.h"
#include "../types.h"

namespace TeensyTimerTool
{
    enum class errModule;
    enum class pitERR;

    // class ErrorHandler
    // {
    //  public:
    //     static int error(unsigned errCode)
    //     {
    //         if (callback != nullptr) callback((int)module, code);
    //         return code;
    //     }
    //     static void attachCallback(errorFunc_t* cb) { callback = cb; }

    //  protected:
    //     static errorFunc_t callback;
    // };

    extern errorFunc_t verboseErrorHandler(Stream&);

    // enum class errModule {
    //     PIT = 1,
    //     MC,
    //     RB
    // };

    // enum class pitErr {
    //     OK,
    //     argErr,
    //     notAllocated,
    //     outOfTimers,

    // };

    // enum class mcErr {
    //     OK,
    //     alrdyMoving,
    // };
}