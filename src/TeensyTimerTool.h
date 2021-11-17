#pragma once

#include "API/oneShotTimer.h"
#include "API/periodicTimer.h"
#include "API/timer.h"
#include "ErrorHandling/error_handler.h"
#include "boardDef.h"

#if defined(TTT_TEENSY4X)
#include "API/pulseGenerator.h"
#endif

//#include "config.h"

static_assert(TEENSYDUINO >= 150, "This library requires Teensyduino > 1.5");