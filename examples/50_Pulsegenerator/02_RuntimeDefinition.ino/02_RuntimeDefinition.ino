#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PulseGenerator pulseGenerator;

void setup()
{
    pulseGenerator.begin(10); // attach the pulse generator to pin 10

    // generate a simple acceleration/decleration pattern at runtime
    // e.g. a stepper motor ramp
    Pulse pulses[200];
    for (int i = 0; i < 100; i++)
    {
        pulses[i].hi = 500ns;
        pulses[i].lo = 2us + 500ns * i;
    }
    for (int i = 0; i < 100; i++)
    {
        pulses[i + 100].hi = pulses[99 - i].hi;
        pulses[i + 100].lo = pulses[99 - i].lo;
    }
    PulseTrain stepperTrain(pulses, sizeof(pulses)/sizeof(pulses[0])); // use the array to construct a PulseTrain

    pulseGenerator.trigger(stepperTrain); // trigger the pulse train.
}

void loop()
{
}
