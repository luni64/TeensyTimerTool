# Choosing a timer

!!! concept inline end "Hardware and software timers"
    A *hardware timer* is a counter in the processor that generates an interrupt when a period has
    expired; its callback runs with very low latency, independent of what the sketch does. A *software
    timer* (TCK) compares a time stamp with the current time whenever the sketch calls `yield()`; it
    needs no hardware, but its callbacks are delayed while the sketch is busy.

In most applications, the timer pool is the right choice: `PeriodicTimer t1;` takes the next free timer,
and the period is limited only by the timer it gets. The timers differ in the longest possible period, the
resolution, the number of channels and a few functions. The following sections help to choose a specific
timer when these properties matter.

## Recommendations

| Requirement | Recommended timers |
|---|---|
| No special requirements | the pool: `PeriodicTimer t1;` |
| Long periods (seconds to hours) | GPT, PIT (up to 179 s), TCK (5 s), TCK64, TCK_RTC (practically unlimited) |
| Very long periods (days) | GPT with prescaler (up to 8.5 days), TCK64, TCK_RTC |
| Short periods, low latency | TMR, GPT, PIT |
| Many timers | TCK (20 channels by default, the number is configurable) |
| `setNextPeriod()`, `PeriodMode::keepElapsed` | PIT, TMR, TCK |
| Callbacks in sync with the real time clock | RTC, TCK_RTC |
| Teensy 3.x | FTM for periods up to 35 … 58 ms, TCK for longer periods |
| Teensy LC | TCK (the only timer available) |

!!! tip "The pool on Teensy 4"
    The first two timers taken from the pool are GPT1 and GPT2 (up to 179 s), the following ones are TMR
    channels, which support periods of up to 55.9 ms only. For several timers with long periods, request
    them explicitly, e.g. `PeriodicTimer t1(TCK);`

## Teensy 4.0, 4.1 and MicroMod

| Timer | Channels | Counter | Resolution | Max. period | One-shot | `setNextPeriod`, `keepElapsed` | Prescaler |
|---|---|---|---|---|---|---|---|
| `GPT1`, `GPT2` | 1 each | 32 bit | 41.7 ns × prescaler | 179 s … 8.5 days | ✓ | – | `PSC_1` … `PSC_4096` |
| `TMR1` … `TMR4` | 4 each | 16 bit | 6.67 ns × prescaler | 437 µs … 55.9 ms | ✓ | ✓ | `PSC_1` … `PSC_128` (default `PSC_128`) |
| `PIT` | 4 | 32 bit | 41.7 ns | 179 s | ✓ | ✓ | – |
| `RTC` | 1 | – | 30.5 µs | 1 s, periods of 2⁻ⁿ s | – | – | – |
| `TCK` | 20 (shared by all TCK types) | 32 bit | 1.67 ns | 5 s | ✓ | ✓ | – |
| `TCK64` | (shared) | 64 bit | 1.67 ns | 634 years | ✓ | ✓ | – |
| `TCK_RTC` | (shared) | 64 bit | 30.5 µs | 634 years | ✓ | ✓ | – |

Values for the default configuration at 600 MHz. `getRemainingTime()` is supported by all timers except
the RTC.

## Teensy 3.x

| Timer | Boards | Channels | Counter | Resolution | Max. period |
|---|---|---|---|---|---|
| `FTM0` | all T3.x | 8 | 16 bit | 0.53 µs (T3.5, T3.6) / 0.67 µs (T3.0 – T3.2 at 96 MHz) | 35 ms / 44 ms |
| `FTM1` | all T3.x | 2 | 16 bit | as FTM0 | as FTM0 |
| `FTM2` | T3.1, T3.2, T3.5, T3.6 | 2 | 16 bit | as FTM0 | as FTM0 |
| `FTM3` | T3.5, T3.6 | 8 | 16 bit | as FTM0 | as FTM0 |
| `TCK` | all T3.x | 20 (shared) | 32 bit | 1 / F_CPU (5.6 ns at 180 MHz) | 5 s |
| `TCK64` | all T3.x | (shared) | 64 bit | 1 / F_CPU | 634 years |

The FTM timers support periodic and one-shot use and `triggerDirect()`, but not `setPeriod()`,
`setNextPeriod()`, the prescaler functions and the getters `getPeriod()`, `getResolution()` and
`getRemainingTime()`. The TCK timers work as on Teensy 4.

!!! warning "Long periods on Teensy 3.x"
    The pool of a Teensy 3 starts with the FTM timers, which support periods of only 35 … 58 ms,
    depending on the board and its clock. For longer periods, `begin()` returns the warning
    `periodOverflow` and the timer runs with its maximum period. Long periods require a software timer:
    `PeriodicTimer t1(TCK);`

## Teensy LC

The Teensy LC provides only the `TCK` timers. They are based on `micros()`: resolution 1 µs, maximum
period 70 minutes.

## The timers in detail

### GPT – General Purpose Timer (Teensy 4)

The two GPT modules (GPT1, GPT2) have one 32 bit channel each. They are clocked with 24 MHz, which gives
periods of up to 179 s with a resolution of 41.7 ns. With the [prescaler](#prescaler), the maximum
period can be extended to 8.5 days. The GPT timers are not used by the Teensyduino core.

```cpp
PeriodicTimer t1(GPT1);
PeriodicTimer t2(GPT2);
```

### TMR – Quad Timer (Teensy 4)

The four TMR modules (TMR1 … TMR4) have four 16 bit channels each, 16 channels in total. They are clocked
with the 150 MHz peripheral clock, divided by a prescaler (default 128: resolution 0.853 µs, maximum
period 55.9 ms). The [prescaler](#prescaler) can be set per channel at runtime. The TMR timers
have the lowest callback latency of all timers (see [Performance](advanced/performance.md)).

```cpp
PeriodicTimer t1(TMR1); // first free channel of TMR1
OneShotTimer  t2(TMR1); // next free channel of TMR1
```

The Teensyduino core uses some TMR channels for `analogWrite()` on certain pins. A TMR timer used by
TeensyTimerTool disables PWM on these pins; see [Avoiding PWM timer clashes](advanced/pwm-timer-clashes.md).

### PIT – Periodic Interrupt Timer (Teensy 4)

The PIT module has four 32 bit channels. They are clocked with 24 MHz (resolution 41.7 ns, maximum period
179 s) and share one interrupt.

```cpp
PeriodicTimer t1(PIT), t2(PIT); // first and second PIT channel
OneShotTimer  t3(PIT);          // third PIT channel
```

!!! warning "PIT and IntervalTimer"
    The `IntervalTimer` of the Teensy core uses the same four PIT channels. TeensyTimerTool and
    `IntervalTimer` each install their own interrupt handler for the PIT and don't know about each
    other's channels, so they disable each other. Don't use the PIT timers of TeensyTimerTool and
    `IntervalTimer` in the same sketch. For this reason, the PIT is not part of the default timer pool.

### RTC – Real Time Clock (Teensy 4)

The RTC timer uses the real time clock to generate periodic callbacks. The periods are limited to
2⁻ⁿ seconds (1 s, 0.5 s, 250 ms, … 30.5 µs), and the callbacks are synchronized with the clock: a 1 s
callback occurs exactly at the second transitions of the RTC. There is one RTC timer; it supports
periodic use only.

```cpp
PeriodicTimer t1(RTC);
// ...
t1.begin([] { digitalToggleFast(LED_BUILTIN); }, 125ms); // toggles the LED every 125 ms, in sync with the RTC
```

### FTM – Flex Timer Module (Teensy 3)

The number of FTM modules and channels depends on the board (see the table above). The channels of a
module share its 16 bit counter; the prescaler is set at compile time (see [Configuration](configuration.md)).

```cpp
PeriodicTimer t1(FTM0), t2(FTM0); // first and second channel of FTM0
OneShotTimer  t3(FTM1);           // first channel of FTM1
```

The Teensyduino core uses the FTM modules for `analogWrite()`; see
[Avoiding PWM timer clashes](advanced/pwm-timer-clashes.md).

### TCK – Tick timers (all boards)

The tick timers are software timers. Instead of a hardware timer, they use the cycle counter of the
processor (Teensy LC: `micros()`) as time base. The library checks them whenever `yield()` is called,
i.e. after each `loop()` iteration and during `delay()`, and invokes the callbacks that are due. How the
tick timers are called can be changed in the [configuration](configuration.md).

```cpp
PeriodicTimer t1(TCK), t2(TCK);
OneShotTimer  t3(TCK);
```

Tick timers need no hardware resources, and they are available on every Teensy. By default, 20 tick
timers can be used at the same time (all TCK types together); the number can be changed in the
[configuration](configuration.md).

The callbacks are only invoked when `yield()` runs. They are delayed by a few µs in a sketch whose
`loop()` runs freely, and by much more if `loop()` blocks for a long time without calling `yield()`.
`delay()` and other functions that call `yield()` are no problem.

### TCK64 and TCK_RTC – 64 bit tick timers

`TCK64` works like `TCK`, but with a 64 bit counter: the maximum period is practically unlimited
(634 years). `TCK_RTC` (Teensy 4) uses the real time clock as time base. Its callbacks are in sync with
the RTC, and its resolution is one RTC tick (30.5 µs).

## Resolution and rounding

Every timer counts ticks of its input clock, so internally a period is always an integer number of
ticks. The requested period is rounded to the nearest number of ticks, with a minimum of one tick.

Example: the TMR timers run on the 150 MHz peripheral clock of the Teensy 4, divided by the default
prescaler of 128. This gives a tick frequency of 150 MHz / 128 = 1.171875 MHz, i.e. a tick period of
0.853 µs. A requested period of 100 µs corresponds to 100 / 0.853 = 117.2 ticks; the timer uses
117 ticks = 99.84 µs. `getPeriod()` returns the period actually used, `getResolution()` the tick period.

The RTC timer supports only periods of 2⁻ⁿ seconds, i.e. 1 s, 0.5 s, 0.25 s, 125 ms, 62.5 ms, … down to
61 µs and 30.5 µs (2⁻¹⁵ s), and selects the next longer one (25 ms → 31.25 ms).

## Prescaler

The TMR and GPT timers have a *prescaler*, which divides the timer clock by a power of two. A larger
prescaler gives longer maximum periods and a coarser resolution:

| Timer | Prescaler | Resolution | Max. period |
|---|---|---|---|
| TMR | `PSC_1` … `PSC_128` (default `PSC_128`) | 6.67 ns … 853 ns | 437 µs … 55.9 ms |
| GPT | `PSC_1` (default) … `PSC_4096` | 41.7 ns … 171 µs | 179 s … 8.5 days |

`setPrescaler()` sets the prescaler of one timer. Called before `begin()`, it takes effect when the timer
starts:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t1(GPT1);
PeriodicTimer t2(TMR1);

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(0, OUTPUT);

    t1.setPrescaler(PSC_4096); // tick 171 µs, max. period 8.5 days
    t1.begin([] { digitalToggleFast(LED_BUILTIN); }, 24h);

    t2.setPrescaler(PSC_1); // tick 6.67 ns, max. period 437 µs
    t2.begin([] { digitalToggleFast(0); }, 10us);
}

void loop()
{
}
```

The example [Prescaler](examples/index.md) shows more details.

On a running timer, a new prescaler takes effect immediately; the period keeps its number of ticks until
the next `setPeriod()`, i.e. it changes with the prescaler. The other timers have no prescaler and return
`errorCode::notImplemented`. The default prescaler of all TMR timers can be changed in the
[configuration](configuration.md); the FTM prescalers of Teensy 3 are set there as well.

