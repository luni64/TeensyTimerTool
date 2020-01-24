#include "error_handler.h"
#include "Arduino.h"
#include "core_pins.h"

namespace TeensyTimerTool
{
    ErrorHandler::ErrorHandler(Stream& s) : stream(s)
    {
        pinMode(LED_BUILTIN, OUTPUT);
    }

    void ErrorHandler::operator()(errorCode code) const
    {
        const char* txt;

        switch (code)
        {
            case errorCode::OK:
                txt = "OK";
                break;

            // warnings
            case errorCode::periodOverflow:
                txt = "Timer period too long, will be clamped to max";
                break;

            // general errors
            case errorCode::reload:
                txt = "Period must not be zero";
                break;
            case errorCode::noFreeChannel:
                txt = "Timer module has no free channel";
                break;
            case errorCode::noFreeModule:
                txt = "Timer pool contains no free timer";
                break;

            default:
                txt = "Unknown error";
                break;
        }

        if ((int)code < 0) // in case of warnings we return after printing
        {
            stream.printf("Warning: %i: %s\n", -(int)code, txt);
            return;
        }

        stream.printf("Error: %i: %s\n", code, txt); // in case of errors we don't return
        while (true)
        {
            digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
            delay(50);
        }
    }
}