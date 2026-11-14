// Changing the prescaler of the TMR (QUAD) and GPT timers with setPrescaler().
//
// The prescaler divides the timer clock: a small prescaler gives a fine resolution but a short
// maximum period, a large one a coarse resolution but long periods.
//
//   TMR: 150 MHz / PSC_1 ... PSC_128   -> tick 6.7 ns ... 853 ns, max. period 437 µs ... 55.9 ms
//        default: TMR_DEFAULT_PSC (PSC_128), can be set per channel
//   GPT:  24 MHz / PSC_1 ... PSC_4096  -> tick 41.7 ns ... 171 µs, max. period 179 s ... 8.5 days
//        default: PSC_1
//
// Periods are rounded to the nearest tick. PIT, TCK and RTC timers have no prescaler
// (setPrescaler() returns errorCode::notSupported).
//
// Pin 0: 10 µs period with PSC_1   -> exactly 10 µs (pin toggles every 10 µs, i.e. 50 kHz)
// Pin 1: 10 µs period with PSC_128 -> 10.24 µs (12 ticks of 0.853 µs)
// LED:   GPT1 with PSC_4096 blinking at 1 Hz; this timer could run periods of up to 8.5 days
// Every 5 s the prescaler of the pin 0 timer is switched between PSC_1 and PSC_16 while it runs.
//
// REQUIRES T4.x

#include "TeensyTimerTool.h"

using namespace TeensyTimerTool;

PeriodicTimer fine(TMR1);   // channel 0 of TMR1
PeriodicTimer coarse(TMR1); // channel 1 of TMR1, the channels of a TMR module have their own prescalers
PeriodicTimer slow(GPT1);

void setup()
{
    pinMode(0, OUTPUT);
    pinMode(1, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    while (!Serial) {}

    // set the prescaler before begin(), begin() applies it
    fine.setPrescaler(PSC_1);
    fine.begin([] { digitalToggleFast(0); }, 10us);

    coarse.begin([] { digitalToggleFast(1); }, 10us); // default prescaler: TMR_DEFAULT_PSC = PSC_128

    slow.setPrescaler(PSC_4096);
    slow.begin([] { digitalToggleFast(LED_BUILTIN); }, 500ms);

    Serial.printf("max. period TMR PSC_1:    %.3f ms\n", fine.getMaxPeriod() * 1E3);
    Serial.printf("max. period TMR PSC_128:  %.3f ms\n", coarse.getMaxPeriod() * 1E3);
    Serial.printf("max. period GPT PSC_4096: %.1f days\n", slow.getMaxPeriod() / 86400);

    // invalid prescaler values are rejected, the timer keeps its prescaler
    errorCode err = coarse.setPrescaler(PSC_256); // TMR: PSC_128 max
    Serial.printf("TMR setPrescaler(PSC_256): %s\n", err == errorCode::invalidArgument ? "errorCode::invalidArgument" : "?");
}

void loop()
{
    static elapsedMillis stopwatch;
    static bool psc16 = false;

    if (stopwatch > 5000)
    {
        stopwatch = 0;
        psc16     = !psc16;

        // On a running timer the new prescaler takes effect immediately. The current period keeps its
        // number of ticks (it would get 16 times longer here), so set the period again.
        fine.setPrescaler(psc16 ? PSC_16 : PSC_1);
        fine.setPeriod(10us);
        Serial.printf("pin 0 timer: PSC_%d, max. period %.3f ms\n", psc16 ? 16 : 1, fine.getMaxPeriod() * 1E3);
    }
}
