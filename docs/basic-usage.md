# Basic usage

This chapter covers the functions needed in most applications: periodic and one-shot timers, starting
and stopping, changing the period, and querying the timer state. The examples use the built-in LED and
run on every Teensy without additional hardware.

## Periodic timers

A `PeriodicTimer` invokes its callback function at a fixed interval. `begin()` sets the callback and the
period and starts the timer:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t1; // next free timer from the pool

void toggleLED()
{
    digitalToggleFast(LED_BUILTIN);
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    t1.begin(toggleLED, 250ms); // call toggleLED every 250 ms
}

void loop()
{
}
```

### Specifying periods

!!! concept inline end "std::chrono durations"
    `std::chrono` is the part of the C++ standard library that deals with time. A *duration* is a time
    span with a unit, written as a number with a suffix: `250ms`, `20us`, `1.5s`, `2min`. Durations can
    be added and compared like numbers. [Reference](https://en.cppreference.com/w/cpp/chrono/duration)

A numeric period is interpreted in **microseconds**; floating point values are accepted. Alternatively,
the period can be given as a `std::chrono` duration. The conversion to microseconds is evaluated by the
compiler, so durations have no runtime cost compared to plain numbers.

```cpp
t1.begin(toggleLED, 250'000); // 250 000 µs (' is the C++ digit separator)
t1.begin(toggleLED, 12.5);    // 12.5 µs
t1.begin(toggleLED, 250ms);   // 250 ms
t1.begin(toggleLED, 20us);    // 20 µs
t1.begin(toggleLED, 1.5s);    // 1.5 s
t1.begin(toggleLED, 2s + 50ms);
t1.begin(toggleLED, 10min);   // only if the timer supports periods of this length
```

!!! concept inline end "Return codes"
    Most functions of the library return an `errorCode`. `errorCode::OK` means success, negative values
    are warnings (the function did its job with a restriction), positive values are errors (the function
    could not do its job). The codes can be checked in the sketch, or reported automatically: see
    [Error handling](error-handling.md).

Each timer has a maximum period. If a longer period is requested, `begin()` returns the warning
`errorCode::periodOverflow` and the timer runs with its maximum period. The maximum periods of all
timers are listed in [Choosing a timer](choosing-a-timer.md).

### Starting and stopping

`stop()` stops the callbacks, `start()` resumes them. The period restarts from the beginning at
`start()`:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t1;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    t1.begin([] { digitalToggleFast(LED_BUILTIN); }, 50ms); // callback as lambda, see Callbacks

    delay(2000);
    t1.stop();  // stop for 2 s
    delay(2000);
    t1.start(); // resume
}

void loop()
{
}
```

To configure a timer without starting it, pass `false` as third argument to `begin()`:

```cpp
t1.begin(toggleLED, 50ms, false); // configured, not running
// ...
t1.start();                       // starts the timer
```

### Changing the period

`setPeriod()` changes the period of a running timer. The new period starts immediately, so the next
callback occurs one new period after the call:

```cpp
t1.begin(toggleLED, 500ms);
// ...
t1.setPeriod(100ms); // next callback in 100 ms, then every 100 ms
```

!!! info "Changed in v1.5"
    Up to v1.4, the TMR and TCK timers included the time already elapsed in the current period. All
    timers now behave identically: the new period starts at the call. The previous behaviour is
    available as an option, see [Keeping the elapsed time](#keeping-the-elapsed-time).

Calling `begin()` on a running timer restarts it with the new callback and period.

## One-shot timers

A `OneShotTimer` invokes its callback once. `begin()` sets the callback, `trigger(delay)` starts the
timer: the callback is invoked when the delay has expired. `trigger()` returns immediately and the sketch
continues; unlike `delay()`, the program is not paused while the timer runs in the background.

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

OneShotTimer ledOff;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    ledOff.begin([] { digitalWriteFast(LED_BUILTIN, LOW); });
}

void loop()
{
    digitalWriteFast(LED_BUILTIN, HIGH);
    ledOff.trigger(10ms); // switch the LED off after 10 ms
    delay(500);
}
```

Behaviour of one-shot timers:

- Calling `trigger()` while a trigger is pending restarts the delay; the callback is invoked once. This is
  the usual way to implement a timeout, e.g. switching a light off 30 s after the last button press.
- `stop()` cancels a pending trigger; the callback is not invoked.
- `trigger()` may be called from the callback itself to chain delays, e.g. to generate pulse sequences.
- `trigger(0)` invokes the callback as soon as possible, i.e. after one timer tick.
- `start()` has no function for one-shot timers and returns `errorCode::notImplemented`.

## Querying the timer

```cpp
float p   = t1.getPeriod();        // period in seconds, as used by the timer
float res = t1.getResolution();    // duration of one timer tick in seconds
float mx  = t1.getMaxPeriod();     // longest possible period in seconds
float rem = t1.getRemainingTime(); // seconds until the next callback, 0 if none is pending
```

All getters return **seconds**. `getPeriod()` and `getResolution()` are available for all Teensy 4
timers, `getRemainingTime()` for the PIT, GPT, TMR and TCK timers. Unsupported getters return `NAN` and
report `errorCode::notImplemented`.

---

## Details

The following sections describe the exact behaviour of the functions above.

### Rounding to timer ticks

Every timer counts ticks of its input clock, so internally a period is always an integer number of
ticks. The requested period is rounded to the nearest number of ticks, with a minimum of one tick.

Example: the TMR timers run on the 150 MHz peripheral clock of the Teensy 4, divided by the default
prescaler of 128. This gives a tick frequency of 150 MHz / 128 = 1.171875 MHz, i.e. a tick period of
0.853 µs. A requested period of 100 µs corresponds to 100 / 0.853 = 117.2 ticks; the timer uses
117 ticks = 99.84 µs. `getPeriod()` returns the period actually used.

The RTC timer supports only periods of 2⁻ⁿ seconds, i.e. 1 s, 0.5 s, 0.25 s, 125 ms, 62.5 ms, … down to
61 µs and 30.5 µs (2⁻¹⁵ s), and selects the next longer one (25 ms → 31.25 ms).

### Callback latency

A hardware timer signals the end of a period with an interrupt. Before the callback starts, the processor
enters the interrupt handler, and the library's handler identifies the timer channel, resets its
interrupt flag and calls the callback. On a Teensy 4.1 at 600 MHz this takes 140 ns for the TMR timers,
160 ns for the GPT and 220 ns for the PIT timers, only slightly more than a hand-written interrupt
handler needs; see the [comparison](advanced/index.md#efficiency).

The software timers (TCK) don't use interrupts. They are evaluated in `yield()`, which runs after each
`loop()` iteration and during `delay()`. Their callbacks are therefore delayed by a few µs, and by
considerably more if `loop()` blocks for a long time. See [Choosing a timer](choosing-a-timer.md) for
details.

### Keeping the elapsed time

```cpp
t1.setPeriod(5ms, PeriodMode::keepElapsed);
```

takes the time already elapsed in the current period into account: if 2 ms have elapsed, the next
callback occurs 3 ms after the call; if more than 5 ms have elapsed, immediately. Supported by the PIT,
TMR and TCK timers; the other timers return `notImplemented`.

### Changing the period at the end of the current period

`setNextPeriod()` completes the current period and uses the new period from then on. Called from the
callback, it generates exact period sequences. The following sketch toggles pin 0 with periods
alternating between 300 µs and 700 µs:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t1(PIT); // PIT, TMR or TCK; the GPT timers don't support setNextPeriod()

void toggle()
{
    static bool shortPeriod = false;
    digitalToggleFast(0);
    shortPeriod = !shortPeriod;
    t1.setNextPeriod(shortPeriod ? 300 : 700); // the period after the current one
}

void setup()
{
    pinMode(0, OUTPUT);
    t1.begin(toggle, 700);
}

void loop()
{
}
```

Supported by the PIT, TMR and TCK timers; the other timers return `notImplemented`.

### Getters with other units

For other units than seconds, specify a `std::chrono` duration as template argument of the getter:

```cpp
auto p = t1.getPeriod<std::chrono::microseconds>();
Serial.printf("period: %lld µs\n", p.count());
```

Integer durations are rounded to the nearest unit (99.84 µs → 100 µs).

### Releasing a timer

A timer releases its channel when the timer object is destroyed, for example a timer declared locally in
a function or an element removed from a `std::vector`. `end()` releases the channel explicitly; a
subsequent `begin()` allocates a new one.

Timer objects cannot be copied, since two objects would then control the same hardware channel. They can
be moved, however, which allows storing them in standard containers; see the example
[TimersInContainers](examples.md).

### Invalid arguments

- Period 0: `begin()`, `setPeriod()` and `setNextPeriod()` return `errorCode::reload`.
- Negative periods or delays: `errorCode::argument`.
- `setPeriod()`, `setNextPeriod()` or `trigger()` before `begin()`: `errorCode::notInitialized`.

In all these cases the timer remains unchanged; a running timer keeps its period and callback. How to
evaluate the returned codes or have them reported automatically is described in
[Error handling](error-handling.md).
