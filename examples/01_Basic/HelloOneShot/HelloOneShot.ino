/********************************************************
 * Basic usage of a one-shot timer
 *
 * setup() switches the LED on and triggers a one-shot timer
 * which switches it off again after 3 s. trigger() returns
 * immediately: loop() keeps running and prints the time and
 * the LED state while the timer runs in the background.
 *
 * The sketch uses a TCK (software) timer, which supports long
 * delays on every Teensy.
 ********************************************************/

#include "TeensyTimerTool.h"

using namespace TeensyTimerTool;

OneShotTimer ledOff(TCK);

void switchOff()
{
    digitalWriteFast(LED_BUILTIN, LOW);
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    while (!Serial && millis() < 3000) {} // wait (max. 3 s) for the serial monitor

    ledOff.begin(switchOff);
    digitalWriteFast(LED_BUILTIN, HIGH);
    ledOff.trigger(3'000'000); // switch the LED off in 3 s (3'000'000 µs)
}

void loop()
{
    Serial.printf("%5lu ms: LED %s\n", millis(), digitalReadFast(LED_BUILTIN) ? "on" : "off");
    delay(250);
}
