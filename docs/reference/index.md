# Reference

This section describes every function of the library in a fixed format: signature, behaviour,
parameters, return values and the timers that support it. The guide chapters ([Usage](../usage.md),
[Callbacks](../callbacks.md), …) explain how to use the functions; this section is the place to look up
exact behaviour.

| Page | Content |
|---|---|
| [PeriodicTimer](periodic-timer.md) | constructor, `begin()`, `setPeriod()`, `setNextPeriod()`, `getPeriod()` |
| [OneShotTimer](one-shot-timer.md) | constructor, `begin()`, `trigger()`, `triggerDirect()`, `getTriggerReload()`, `start()` |
| [Common functions](common.md) | `start()`, `stop()`, `end()`, `setPrescaler()`, `getMaxPeriod()`, `getResolution()`, `getRemainingTime()`, getters with units, copying and moving |
| [Timer types](timer-types.md) | the timer types per board, the functions each of them supports, the default pools |
| [Errors and other functions](errors.md) | `errorCode`, `attachErrFunc()`, `ErrorHandler`, `tick()`, `PeriodMode`, prescaler constants |

## Conventions

- **Namespace:** all names are declared in the namespace `TeensyTimerTool`.
- **Periods and delays** are given in microseconds (any arithmetic type, e.g. `500`, `12.5`) or as
  `std::chrono` duration (`500us`, `2ms`, `1.5s`). The conversion of a duration is evaluated at compile
  time.
- **Return values:** most functions return an `errorCode`. `OK` (0) means success; negative values are
  warnings, the function did its job with a restriction; positive values are errors, the function could
  not do its job. Every result other than `OK` is also passed to the error function, if one is attached
  (see [Error handling](../error-handling.md)).
- **Supported timers** lists the timers on which a function works. On the others it returns
  `errorCode::notImplemented` and doesn't change the timer.
- **Getters** return seconds as `float`, or the value in the `std::chrono` duration given as template
  parameter.
