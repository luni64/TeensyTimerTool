#include "TeensyTimerTool.h"

using namespace TeensyTimerTool;

void callback(int& someInt, int cnt)  // this callback has context, i.e. parameter
{
    for (int i = 0; i < cnt; i++)     // whenn called, print out someInt cnt times
    {
        Serial.print(someInt);
        Serial.print(" | ");
    }
    Serial.println();
}

//==============================================================

Timer t;
int number = 0;

void setup()
{
    t.beginPeriodic([] { callback(number, 5); }, 50'000);
}

void loop()
{
    number++;     // change every second
    delay(1000);
}