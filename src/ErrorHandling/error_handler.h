#pragma once

#include "Stream.h"
#include "error_codes.h"

namespace TeensyTimerTool
{
    /// @brief Ready-made error function for attachErrFunc(): `attachErrFunc(ErrorHandler(Serial));`
    /// @details Warnings are printed as "W-<code>: <text>" and the sketch continues. Errors are printed as
    /// "E-<code>: <text>", then the handler blinks LED_BUILTIN in an endless loop.
    class ErrorHandler
    {
     public:
        /// @brief Creates the handler; sets LED_BUILTIN to output.
        /// @param s stream for the messages, e.g. Serial
        ErrorHandler(Stream &s);

        /// @brief Prints the message for `code`; for errors it doesn't return (blinks LED_BUILTIN).
        void operator()(errorCode code) const;

     protected:
        Stream &stream;
    };
} // namespace TeensyTimerTool
