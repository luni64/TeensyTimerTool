
#include "TeensyTimerTool.h"
#include "pins.h"

using namespace TeensyTimerTool;
using namespace pins;

pin<13> LED(OUTPUT);
Timer timer;

void setup()
{
    timer.beginOneShot([] { LED = LOW; });
}

void loop()
{
    LED = HIGH;
    timer.trigger(25'000);
    delay(1'000);
}
