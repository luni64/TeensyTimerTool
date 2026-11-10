# Basic usage

This chapter shows the everyday functions: starting periodic and one-shot timers, stopping them, changing
the period and reading back what the timer does. All examples run on every Teensy; they use the built-in
LED, so you don't need any extra hardware.

## Periodic timers

A `PeriodicTimer` calls its callback again and again. `begin()` sets the callback and the period and
starts the timer:

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

### Periods: numbers or units

A plain number is a period in **microseconds**, floats are fine as well. Or you use the time units of
`std::chrono`, which are easier to read:

```cpp
t1.begin(toggleLED, 250'000); // 250'000 µs (the ' is just a separator, like 250,000)
t1.begin(toggleLED, 12.5);    // 12.5 µs
t1.begin(toggleLED, 250ms);   // 250 ms
t1.begin(toggleLED, 20us);    // 20 µs
t1.begin(toggleLED, 1.5s);    // 1.5 s
t1.begin(toggleLED, 2s + 50ms);
t1.begin(toggleLED, 10min);   // if the timer can do that long, see below
```

Every timer has a maximum period. If you ask for more, `begin()` returns the warning
`errorCode::periodOverflow` and the timer runs with its maximum period. Which timer can do how long is
listed in [Choosing a timer](choosing-a-timer.md).

### Starting and stopping

`stop()` pauses a timer, `start()` lets it run again. After `start()` the period starts from the
beginning:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t1;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    t1.begin([] { digitalToggleFast(LED_BUILTIN); }, 50ms); // fast blinking, the callback is a lambda (see Callbacks)

    delay(2000);
    t1.stop(); // pause for 2 s
    delay(2000);
    t1.start(); // blink again
}

void loop()
{
}
```

If you want to prepare a timer in `setup()` but start it later, pass `false` as third parameter of
`begin()`:

```cpp
t1.begin(toggleLED, 50ms, false); // ready, but not running
// ...
t1.start();                       // now it runs
```

### Changing the period

`setPeriod()` changes the period of a running timer. The new period starts right away, i.e. the next
callback comes one new period after the call:

```cpp
t1.begin(toggleLED, 500ms);
// ...
t1.setPeriod(100ms); // next callback in 100 ms, then every 100 ms
```

!!! info "Changed in v1.5"
    Up to v1.4 the TMR and TCK timers counted the time already elapsed in the current period. Now all
    timers behave the same: the new period starts at the call. If you need the old behaviour, see
    [Keeping the elapsed time](#keeping-the-elapsed-time) below.

`begin()` on a running timer works as well: it restarts the timer with the new callback and period.

## One-shot timers

A `OneShotTimer` calls its callback only once, `delay` after you call `trigger(delay)`. `begin()` only
sets the callback, it doesn't start anything:

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
    ledOff.trigger(10ms); // switch the LED off in 10 ms
    delay(500);
}
```

Good to know about one-shot timers:

- `trigger()` while the timer is still waiting starts the delay again; the callback is called only once.
  This makes a nice timeout, e.g. "switch the light off 30 s after the last button press".
- `stop()` cancels a pending trigger, the callback is not called.
- You can call `trigger()` in the callback itself to chain delays, e.g. to generate a pulse pattern.
- `trigger(0)` calls the callback as soon as possible (after one timer tick).
- `start()` does nothing for one-shot timers (it returns `errorCode::notImplemented`), use `trigger()`.

## Reading back the timer

```cpp
float p   = t1.getPeriod();        // the period in seconds, as the timer actually runs it
float res = t1.getResolution();    // one timer tick in seconds
float mx  = t1.getMaxPeriod();     // the longest possible period in seconds
float rem = t1.getRemainingTime(); // seconds until the next callback, 0 if none is pending
```

All of them return **seconds**. If you prefer other units, ask for a `std::chrono` duration:

```cpp
auto p = t1.getPeriod<std::chrono::microseconds>();
Serial.printf("period: %lld µs\n", p.count());
```

`getPeriod()` and `getResolution()` exist for periodic timers on all Teensy 4 timers, `getRemainingTime()`
on PIT, GPT, TMR and TCK timers. Where a function isn't supported, it returns `NAN` (and the error
`notImplemented`).

## Freeing a timer

A timer releases its hardware channel when the timer object is destroyed, e.g. a timer declared inside a
function, or one removed from a `std::vector`. You can also release it explicitly with `end()`; a later
`begin()` takes a new channel.

Timer objects can't be copied (two objects would own the same hardware), but they can be moved. That's
why you can keep them in containers: see the example
[TimersInContainers](examples.md).

---

## Details

The rest of this chapter is for when you need the exact behaviour.

### Periods are rounded to timer ticks

Every timer counts ticks of its clock, and a period is always a whole number of ticks: the requested
period is rounded to the nearest tick, but at least one tick. Most of the time you won't notice. A TMR
timer with its default prescaler has a tick of 0.853 µs, so `begin(cb, 100)` gives 117 ticks =
99.84 µs. `getPeriod()` tells you the value actually used. The RTC timer can only do periods of
2⁻ⁿ seconds and picks the next longer one (25 ms → 31.25 ms).

### Delay of the callbacks

On the hardware timers the callback starts about 0.2 … 1.5 µs after the timer event. The software timers
(TCK) are checked whenever `yield()` runs, i.e. after each `loop()` and inside `delay()`; their callbacks
come a few µs late, more if your `loop()` blocks for a long time. Details: [Choosing a timer](choosing-a-timer.md).

### Keeping the elapsed time

```cpp
t1.setPeriod(5ms, PeriodMode::keepElapsed);
```

counts the time already elapsed in the current period: if 2 ms have passed, the next callback comes 3 ms
later; if more than 5 ms have passed, right away. Supported by PIT, TMR and the TCK timers, the others
return `notImplemented`.

### Changing the period at the end of the current one

```cpp
t1.setNextPeriod(2ms);
```

lets the current period finish and uses the new period from then on. Called in the callback, this gives
exact period patterns, e.g. a pin that is 300 µs high and 700 µs low. Supported by PIT, TMR and the TCK
timers, the others return `notImplemented`.

### Invalid values

- Period 0: `begin()`, `setPeriod()` and `setNextPeriod()` return `errorCode::reload`.
- Negative periods or delays return `errorCode::argument`.
- `setPeriod()`, `setNextPeriod()` and `trigger()` before `begin()` return `errorCode::notInitialized`.

In all these cases the timer is not changed: a running timer keeps its period and callback. How to check
the returned codes or get them reported automatically: [Error handling](error-handling.md).
