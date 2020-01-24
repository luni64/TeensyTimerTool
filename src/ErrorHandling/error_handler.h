#pragma once

#include "error_codes.h"
#include "Stream.h"

namespace TeensyTimerTool
{
    class ErrorHandler
    {
     public:
        ErrorHandler(Stream& s);
        void operator()(errorCode code) const;

     protected:
         Stream& stream;
    };
}