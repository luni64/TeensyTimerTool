
#include "TeensyTimerTool.h"
#include "pins.h"

using namespace TeensyTimerTool;
using namespace pins;

pin<13> LED(OUTPUT);
OneShotTimer timer;

void setup()
{
    timer.begin([] { LED = LOW; });
}

void loop()
{
    LED = HIGH;
    timer.trigger(25'000);
    delay(1'000);
}
