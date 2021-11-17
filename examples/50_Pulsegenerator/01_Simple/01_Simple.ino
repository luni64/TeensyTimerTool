#include "Arduino.h"
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PulseGenerator pulseGenerator;

// define a pulse train {{high,low},{high,low},....}
PulseTrain train_1{{2ms, 3ms}, {1ms, 3ms}, {50us, 3ms}, {1000, 3000}, {2000, 3000}}; // numbers without units are interpreted as us (microseconds)

// another pulse train (5x 100ns pulse with 1µs spacing)
PulseTrain train_2{{300ns, 2us}, {300ns, 3us}, {50ns, 3us}, {300ns, 2us}, {300ns, 2us}};

void setup()
{
    pulseGenerator.begin(10); // attach the pulse generator to pin 10
}

void loop()
{
    pulseGenerator.trigger(train_1);    // select one or both pulse trains
    delay(100);
    //pulseGenerator.trigger(train_2);
    //delay(100);
}
