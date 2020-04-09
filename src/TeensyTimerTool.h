#pragma once

#include "config.h"
#include "timer.h"
#include "periodicTimer.h"
#include "oneShotTimer.h"
#include "ErrorHandling/error_handler.h"

static_assert(TEENSYDUINO >= 150, "This library requires Teensyduino > 1.5");