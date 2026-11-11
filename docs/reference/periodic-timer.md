# PeriodicTimer

```cpp
class PeriodicTimer; // namespace TeensyTimerTool
```

A `PeriodicTimer` invokes its callback at a fixed interval.

```cpp
PeriodicTimer t1;      // takes the next free timer from the pool in begin()
PeriodicTimer t2(PIT); // takes a channel of the given timer type in begin()
```

The functions common to all timers (`begin()`, `start()`, `stop()`, `end()`, getters) are described in
Common functions. This page describes the functions specific to periodic timers.

<!-- TODO: setPeriod(), getPeriod() -->

---

### setNextPeriod()

```cpp
template <typename period_t>
errorCode setNextPeriod(period_t period);
```

Sets the period that starts when the current period ends. The current period is not changed: the next
callback occurs as scheduled, the callbacks after it at the new interval.

Called from the callback, `setNextPeriod()` generates exact period sequences, since the new value is in
place long before the current period ends. If it is called several times during one period, the last
value is used.

| Parameter | |
|---|---|
| `period` | the new period, in microseconds or as `std::chrono` duration. Rounded to the nearest number of timer ticks, at least one tick. |

| Return value | Condition |
|---|---|
| `OK` | the new period is set |
| `periodOverflow` (warning) | `period` is longer than `getMaxPeriod()`; the maximum period is used |
| `reload` | `period` is 0; the timer is unchanged |
| `argument` | `period` is negative; the timer is unchanged |
| `notInitialized` | called before `begin()` |
| `notImplemented` | the timer doesn't support `setNextPeriod()`; the timer is unchanged |

**Supported timers:** PIT, TMR1 … TMR4, TCK, TCK64, TCK_RTC.
Not supported: GPT1, GPT2, RTC (Teensy 4), FTM0 … FTM3 (Teensy 3).

**Notes**

- `getPeriod()` returns the new period immediately after the call, i.e. the latest period set, not the
  period currently running.
- Intended for running timers. On a stopped timer, whether the first period after `start()` already uses
  the new value depends on the timer; use `setPeriod()` to change the period of a stopped timer.
- `setNextPeriod()` can be called from the timer's own callback.

**See also:** `setPeriod()`, [Changing the period at the end of the current period](../basic-usage.md#changing-the-period-at-the-end-of-the-current-period)
(example: alternating periods).

<!-- tests: api_set_next_period, la_next_period, api_period_zero, api_negative_values,
     api_set_period_before_begin, api_get_period, api_error_posted_once, api_max_period_and_overflow -->
