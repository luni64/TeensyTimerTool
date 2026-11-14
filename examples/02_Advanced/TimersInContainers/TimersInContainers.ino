// Timers in STL containers: a set of blinking pins, added and removed at runtime over the serial monitor.
//
// Every blinker owns a PeriodicTimer and lives in a std::vector. Timers can't be copied, but they can be
// moved: when the vector grows or an element is erased, the timers are moved and keep running. An erased
// timer stops, and its timer channel is free again.
//
// Commands (serial monitor, end with newline):
//   a <pin> <ms>   add a blinker, e.g. "a 13 250" toggles the LED every 250 ms
//   r <pin>        remove the blinker of this pin
//   l              list the blinkers
//
// No hardware needed: every second the sketch prints how often each pin toggled. With an LED or a scope
// on the pins you see the same. Each 'a' takes a timer from the timer pool (see defaultConfig.h);
// when the pool is empty, begin() returns errorCode::poolExhausted. A period longer than the timer allows
// gives the warning errorCode::periodOverflow, the timer then runs with its max. period (T3.x: the pool
// starts with the FTM timers, max. ~35 ms, so already the 250 ms LED at startup is clipped).

#include "TeensyTimerTool.h"
#include <vector>

using namespace TeensyTimerTool;

struct Blinker
{
    unsigned pin;
    float ms;            // period
    PeriodicTimer timer; // default constructor: takes the next free timer from the pool in begin()
};

std::vector<Blinker> blinkers;

volatile uint32_t toggles[CORE_NUM_DIGITAL]; // incremented by the callbacks, read by the status output
uint32_t lastToggles[CORE_NUM_DIGITAL];

Blinker* findBlinker(unsigned pin)
{
    for (auto& b : blinkers)
        if (b.pin == pin) return &b;
    return nullptr;
}

void addBlinker(unsigned pin, float ms)
{
    if (pin >= CORE_NUM_DIGITAL || ms <= 0) { Serial.println("usage: a <pin> <ms>"); return; }
    if (findBlinker(pin)) { Serial.printf("pin %u blinks already, remove it first\n", pin); return; }

    pinMode(pin, OUTPUT);
    blinkers.push_back({pin, ms, PeriodicTimer()}); // may reallocate: the running timers are moved

    // Capture the pin by value. Don't capture a reference or pointer to the vector element: it becomes
    // invalid as soon as the vector reallocates or an element in front of it is erased.
    errorCode err = blinkers.back().timer.begin([pin] { digitalWriteFast(pin, !digitalReadFast(pin)); toggles[pin]++; }, ms * 1000);

    if (err == errorCode::poolExhausted)
    {
        blinkers.pop_back();
        Serial.println("no free timer left in the pool");
        return;
    }
    if (err == errorCode::periodOverflow) // warning only, the timer runs with its max. period
    {
        blinkers.back().ms = blinkers.back().timer.getMaxPeriod() * 1000;
        Serial.printf("%g ms is too long for this timer, using its max. period\n", ms);
    }
    Serial.printf("pin %u: blinking every %g ms (%u blinkers)\n", pin, blinkers.back().ms, blinkers.size());
}

void removeBlinker(unsigned pin)
{
    Blinker* b = findBlinker(pin);
    if (!b) { Serial.printf("no blinker on pin %u\n", pin); return; }

    blinkers.erase(blinkers.begin() + (b - blinkers.data())); // destroys its timer: stopped, channel free again
    digitalWriteFast(pin, LOW);
    Serial.printf("pin %u removed (%u blinkers)\n", pin, blinkers.size());
}

void listBlinkers()
{
    if (blinkers.empty()) Serial.println("no blinkers, add one with e.g. 'a 13 250'");
    for (auto& b : blinkers) Serial.printf("  pin %2u: every %g ms\n", b.pin, b.ms);
}

void handleCommand(char* line)
{
    char cmd = 0;
    unsigned pin = 0;
    float ms = 0;
    sscanf(line, " %c %u %f", &cmd, &pin, &ms);
    switch (cmd)
    {
        case 'a': addBlinker(pin, ms); break;
        case 'r': removeBlinker(pin); break;
        case 'l': listBlinkers(); break;
        default: Serial.println("commands: a <pin> <ms>, r <pin>, l");
    }
}

void printStatus() // toggles per pin in the last second
{
    if (blinkers.empty()) return;
    Serial.print("toggles in the last second (pin:count):");
    for (auto& b : blinkers)
    {
        uint32_t n = toggles[b.pin];
        Serial.printf("  %u:%lu", b.pin, n - lastToggles[b.pin]);
        lastToggles[b.pin] = n;
    }
    Serial.println();
}

void setup()
{
    while (!Serial) {}
    Serial.println("Timers in a std::vector. Commands: a <pin> <ms>, r <pin>, l");
    addBlinker(LED_BUILTIN, 250); // start with the LED blinking
}

void loop()
{
    static char line[40];
    static unsigned len = 0;
    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n' || c == '\r')
        {
            line[len] = 0;
            if (len > 0) handleCommand(line);
            len = 0;
        } else if (len < sizeof(line) - 1)
            line[len++] = c;
    }

    static elapsedMillis stopwatch;
    if (stopwatch >= 1000)
    {
        stopwatch -= 1000;
        printStatus();
    }
}
