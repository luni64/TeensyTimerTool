# Upgrading from 1.4

Version 1.5 keeps the interface of 1.4: existing sketches and configuration files (`userConfig.h`)
compile without changes, apart from the cases listed below. Some behaviour, which was undefined or
differed between the timer types, is now defined; sketches relying on the previous behaviour may need
small adaptations.

## Changes that may need adaptations

### `setPeriod()` restarts the period on all timers

`setPeriod(p)` now always starts the new period at the call. Up to 1.4, the TMR and TCK timers counted
the time already elapsed in the current period (PIT, GPT and RTC restarted already). To keep the previous
behaviour of the TMR and TCK timers, pass `PeriodMode::keepElapsed`:

```cpp
t1.setPeriod(5ms, PeriodMode::keepElapsed); // behaviour of TMR / TCK timers up to 1.4
```

### Timers can't be copied

Copying a timer object doesn't compile any more. A copy of a started timer released its channel twice,
which crashed the sketch, e.g. when a `std::vector<PeriodicTimer>` grew. Timers can be moved instead:
pass them by reference, or use `std::move`. Containers of timers work.

### Periods are rounded to the nearest tick

The PIT, GPT, TMR and TCK timers round periods and delays to the nearest timer tick; up to 1.4, they
truncated them, and TMR periods were one tick too long. Periods change by up to one tick: a 100 µs period
on a TMR timer with the default prescaler gave 100.69 µs (118 ticks), now 99.84 µs (117 ticks). RTC periods are now
rounded up to the next 2⁻ⁿ s as documented (1 ms gives 1.95 ms; 1.4 gave 976.6 µs).

### `begin(callback, period, false)` on TMR timers

`begin()` with `start = false` no longer starts TMR timers; it did, against the documentation.

### Defined return codes for invalid arguments

- Negative periods and delays return `errorCode::argument`, period 0 returns `errorCode::reload`. The
  timer is unchanged in both cases.
- `OneShotTimer::start()` returns `errorCode::notImplemented` and doesn't invoke the callback (in 1.4,
  it invoked the callback).
- Too long periods return the warning `errorCode::periodOverflow` on all timers (in 1.4 only TMR); the
  timer runs with its maximum period.
- `begin()` after `setPrescaler()` on a timer without prescaler returns the warning
  `errorCode::prescalerIgnored` instead of the error `notImplemented`; the timer runs in both cases.
- The error function attached with `attachErrFunc()` is called exactly once per failed call (some calls
  invoked it twice).

### `TMR_DEFAULT_PSC`

A value outside `PSC_1` … `PSC_128` in the configuration is a compile error; it was silently truncated.

## New functions

- `setPrescaler()` for TMR and GPT timers, at runtime; see [Prescaler](choosing-a-timer.md#prescaler).
- `setPeriod(p, PeriodMode::keepElapsed)` for PIT, TMR and TCK timers.
- `getPeriod()`, `getResolution()`: the period and tick actually used. `getRemainingTime()` for PIT,
  GPT and TMR timers (TCK only in 1.4).
- All getters optionally return `std::chrono` durations: `t1.getPeriod<std::chrono::microseconds>()`.
- `end()`: stops a timer and releases its channel.
- Timers can be moved and kept in containers.
- `OneShotTimer::stop()` cancels a pending trigger.
- `triggerDirect()` and `getTriggerReload()` for PIT and TMR timers.

## Bug fixes

Version 1.5 fixes a number of bugs, among them: destroying a PIT or RTC timer object crashed the sketch;
a stopped TMR channel still invoked its callback when another channel of the same module fired;
destroyed FTM timers (Teensy 3) kept running; PIT periods longer than about 179 µs were limited to 179 µs
in 1.4.2 and 1.4.3. The complete list is in the
[changelog](https://github.com/luni64/TeensyTimerTool/releases).
