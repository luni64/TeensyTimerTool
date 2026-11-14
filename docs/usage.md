# Usage

This chapter covers the functions needed in most applications: periodic and one-shot timers, starting
and stopping, changing the period, querying the timer state, and arrays of timers. The examples use the built-in LED and
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

A numeric period is interpreted in **microseconds**; floating point values are accepted. Alternatively,
the period can be given as a `std::chrono` duration (1). The conversion to microseconds is evaluated by
the compiler, so durations have no runtime cost compared to plain numbers.
{ .annotate }

1.  **std::chrono durations**

    `std::chrono` is the part of the C++ standard library that deals with time. A *duration* is a time
    span with a unit, written as a number with a suffix: `250ms`, `20us`, `1.5s`, `2min`. Durations can
    be added and compared like numbers. [Reference](https://en.cppreference.com/w/cpp/chrono/duration)

```cpp
t1.begin(toggleLED, 250'000); // 250 000 µs (' is the C++ digit separator)
t1.begin(toggleLED, 12.5);    // 12.5 µs
t1.begin(toggleLED, 250ms);   // 250 ms
t1.begin(toggleLED, 20us);    // 20 µs
t1.begin(toggleLED, 1.5s);    // 1.5 s
t1.begin(toggleLED, 2s + 50ms);
t1.begin(toggleLED, 10min);   // only if the timer supports periods of this length
```

Each timer has a maximum period. If a longer period is requested, `begin()` returns the warning
`errorCode::periodOverflow` (1) and the timer runs with its maximum period. The maximum periods of all
timers are listed in [Choosing a timer](choosing-a-timer.md).
{ .annotate }

1.  **Return codes**

    Most functions of the library return an `errorCode`. `errorCode::OK` means success, negative values
    are warnings (the function did its job with a restriction), positive values are errors (the function
    could not do its job). The codes can be checked in the sketch, or reported automatically: see
    [Error handling](error-handling.md).

### Starting and stopping

`stop()` stops the callbacks, `start()` starts them again. The period restarts from the beginning at
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
    t1.start(); // start again, with a full period
}

void loop()
{
}
```

`resume()` instead continues the interrupted period, as if the timer had not been stopped: a timer
stopped 30 ms into a 50 ms period invokes its callback 20 ms after `resume()`
(see [`resume()`](reference/periodic-timer.md#resume); not available for the RTC timer).

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

#### Changing the period at the end of the current period

`setNextPeriod()` completes the current period and uses the new period from then on. Called from the
callback, it generates exact period sequences. The following sketch toggles pin 0 with periods
alternating between 300 µs and 700 µs:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t1(PIT); // Teensy 4: PIT or TMR, Teensy 3: TCK (GPT and FTM don't support setNextPeriod())

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

#### Keeping the elapsed time

By default, `setPeriod()` discards the time elapsed in the current period: the new period starts at the
call. With the parameter `PeriodMode::keepElapsed`, the elapsed time counts towards the new period:

```cpp
t1.setPeriod(5ms, PeriodMode::keepElapsed);
```

If 2 ms of the current period have elapsed, the next callback occurs 3 ms after the call, i.e. 5 ms after
the previous callback. If more than 5 ms have elapsed already, the callback occurs immediately. The
following periods are 5 ms.

This is useful when the period is adjusted frequently, e.g. the step rate of a stepper motor that is
updated every millisecond during acceleration. With the default behaviour, every update would delay the
next step by up to one period; with `keepElapsed`, the steps follow the new rate without gaps.

`setNextPeriod()` and `keepElapsed` are supported by the PIT, TMR and TCK timers. The other timers return
`errorCode::notSupported`; see [Choosing a timer](choosing-a-timer.md).

## One-shot timers

A `OneShotTimer` invokes its callback once. `begin()` sets the callback, `trigger(delay)` starts the
timer: the callback is invoked when the delay has expired. `trigger()` returns immediately and the sketch
continues; unlike `delay()`, the program is not paused while the timer runs in the background.

The following sketch implements a timeout: every character received from the serial monitor switches the
LED on, and the LED goes off 2 s after the last character. Each `trigger()` restarts the delay.

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

OneShotTimer ledOff(TCK); // software timer, supports long delays on every Teensy

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    ledOff.begin([] { digitalWriteFast(LED_BUILTIN, LOW); }); // callback as lambda, see Callbacks
}

void loop()
{
    if (Serial.available())
    {
        while (Serial.available()) Serial.read(); // discard the input
        digitalWriteFast(LED_BUILTIN, HIGH);
        ledOff.trigger(2s); // (re)start the 2 s timeout
    }
}
```

Behaviour of one-shot timers:

- Calling `trigger()` while a trigger is pending restarts the delay; the callback is invoked once, as in
  the timeout example above.
- `stop()` cancels a pending trigger; the callback is not invoked.
- `trigger()` may be called from the callback itself to chain delays, e.g. to generate pulse sequences.
- `trigger(0)` invokes the callback as soon as possible, i.e. after one timer tick.
- `start()` has no function for one-shot timers and returns `errorCode::notSupported`.

## Querying the timer

```cpp
float p   = t1.getPeriod();        // period in seconds, as used by the timer
float res = t1.getResolution();    // duration of one timer tick in seconds
float mx  = t1.getMaxPeriod();     // longest possible period in seconds
float rem = t1.getRemainingTime(); // seconds until the next callback, 0 if none is pending
```

All getters return **seconds**. `getPeriod()` returns the period the timer actually uses, which can
differ slightly from the requested one: every timer counts ticks of its clock, so periods are rounded to
whole ticks (see [Resolution and rounding](choosing-a-timer.md#resolution-and-rounding)).

`getPeriod()` and `getResolution()` are available for all Teensy 4 timers, `getRemainingTime()` for the
PIT, GPT, TMR and TCK timers. Unsupported getters return `NAN` and report `errorCode::notSupported`.

## Arrays and containers of timers

Timers can be kept in arrays, which is convenient for a number of similar tasks. The timer type of each
element is given in the braces. The following sketch uses three software timers with different periods:

```cpp
--8<-- "01_Basic/ArrayOfTimersSimple/ArrayOfTimersSimple.ino"
```

Output:

```
 1396 ms: timer 0
 2396 ms: timer 0
 2396 ms: timer 1
 3396 ms: timer 0
 3396 ms: timer 2
 4396 ms: timer 0
 4396 ms: timer 1
...
```

The timer types in an array can be mixed. The following sketch (Teensy 4) generates pulses of
different lengths with a GPT, two TMR and two software timers:

```cpp
--8<-- "01_Basic/ArrayOfTimers/ArrayOfTimers.ino"
```

Timers can also be stored in standard containers like `std::vector`, and added or removed while the
sketch runs. A timer that is removed from the container stops automatically and releases its channel;
the other timers keep running. The following sketch starts three timers and removes the second one
after 5 s:

```cpp
#include "TeensyTimerTool.h"
#include <vector>
using namespace TeensyTimerTool;

std::vector<PeriodicTimer> timers;

void setup()
{
    for (int i = 0; i < 3; i++)
    {
        timers.emplace_back(TCK); // add a software timer to the container
        timers.back().begin([i] { Serial.printf("timer %d\n", i); }, 1s);
    }

    delay(5000);
    timers.erase(timers.begin() + 1); // remove the second timer: it stops
    Serial.println("timer 1 removed");
}

void loop()
{
}
```

Output:

```
timer 0
timer 1
timer 2
...
timer 1 removed
timer 0
timer 2
```

The example `TimersInContainers` shows a larger application: a set of blinking pins which are added and
removed from the serial monitor.

---

The exact behaviour of all functions, including their return codes for invalid arguments, is described
in the [Reference](reference/index.md). Callback latency and CPU load are covered in
[Performance](advanced/performance.md).
