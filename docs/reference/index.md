# Reference

This section describes every function of the library in a fixed format: signature, behaviour,
parameters, return values and the timers that support it. The guide chapters ([Usage](../usage.md),
[Callbacks](../callbacks.md), …) explain how to use the functions; this section is the place to look up
exact behaviour.

!!! note "Work in progress"
    The reference is being written. Finished: [`PeriodicTimer::setNextPeriod()`](periodic-timer.md#setnextperiod).

## Classes

- [PeriodicTimer](periodic-timer.md): timers which invoke their callback at a fixed interval.
- OneShotTimer: timers which invoke their callback once after a delay.
- Common functions: `begin()`, `start()`, `stop()`, `end()`, `setPrescaler()` and the getters.

## Types

- Error codes: all values of `errorCode`, their meaning and the functions returning them.
- Timer types: `PIT`, `GPT1`, `TMR1`, `TCK` …, with the functions each of them supports.

## Conventions

- **Periods and delays** are given in microseconds (any arithmetic type, e.g. `500`, `12.5`) or as
  `std::chrono` duration (`500us`, `2ms`, `1.5s`). The conversion of a duration is evaluated at compile
  time.
- **Return values:** most functions return an `errorCode`. `OK` (0) means success; negative values are
  warnings, the function did its job with a restriction; positive values are errors, the function could
  not do its job. Every result other than `OK` is also passed to the error handler, if one is attached
  (see [Error handling](../error-handling.md)).
- **Supported timers** lists the timers on which a function works. On the others it returns
  `errorCode::notImplemented` and doesn't change the timer.
