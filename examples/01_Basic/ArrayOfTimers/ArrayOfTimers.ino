#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

// Each element of the array gets its own timer type, the types can be mixed (here: Teensy 4)
OneShotTimer t[]{GPT1, TMR1, TMR1, TCK, TCK}; // GPT1, two channels of TMR1 and two software timers
//OneShotTimer t[5];                          // alternative: 5 timers from the pool (all boards)

constexpr unsigned nrOfTimers = sizeof(t) / sizeof(t[0]);

void setup()
{
    // setup timers, use lambdas as callbacks
    for (unsigned i = 0; i < nrOfTimers; i++)
    {
        pinMode(i, OUTPUT);
        t[i].begin([i] { digitalWriteFast(i, LOW); }); // callback resets pin to LOW
    }
}

void loop()
{
    for (unsigned i = 0; i < nrOfTimers; i++)
    {
        digitalWriteFast(i, HIGH);
        t[i].trigger(50 * (i + 1)); // 50, 100, 150 ... µs
    }
    delay(1);
}
