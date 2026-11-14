#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t[]{TCK, TCK, TCK}; // array of three software timers, available on all boards

void onTimer0() { Serial.printf("%5lu ms: timer 0\n", millis()); }
void onTimer1() { Serial.printf("%5lu ms: timer 1\n", millis()); }
void onTimer2() { Serial.printf("%5lu ms: timer 2\n", millis()); }

void setup()
{
    while (!Serial && millis() < 3000) {} // wait (max. 3 s) for the serial monitor

    t[0].begin(onTimer0, 1s); // the elements of the array are used like single timers
    t[1].begin(onTimer1, 2s);
    t[2].begin(onTimer2, 3s);
}

void loop()
{
}
