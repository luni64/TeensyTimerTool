#include "TeensyTimerTool.h"

using namespace TeensyTimerTool;

// Some Examples for timer arrays

//Timer t[]{FTM0, FTM0, FTM2};                             // two channels from FTM0, one from FTM2
//Timer t[3];                                              // use 3 timers from the pool
Timer t[]{FTM0, FTM0, FTM0, FTM0, FTM0, FTM0, FTM0, FTM0}; // all 8 channels of FTM0

constexpr unsigned nrOfTimers = sizeof(t)/sizeof(t[0]);

void setup()
{
    // Each timer handles one pin
    for (unsigned i = 0; i < nrOfTimers; i++)
    {
        pinMode(i, OUTPUT);
    }

    // setup timers, use lambdas as callbacks
    for (unsigned i = 0; i < nrOfTimers; i++)
    {
        t[i].beginOneShot([i] { digitalWriteFast(i, LOW); }); // lambda captures pin number i
    }
}

void loop()
{
    for (unsigned i = 0; i < nrOfTimers; i++)
    {
        digitalWriteFast(i, HIGH);
        t[i].trigger(50 * (i + 1));  // 50, 100, 150 ... µs
    }
    delay(1);
}
