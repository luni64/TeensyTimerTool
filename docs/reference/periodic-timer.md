# PeriodicTimer

```cpp
class PeriodicTimer; // namespace TeensyTimerTool
```

A `PeriodicTimer` invokes its callback at a fixed interval. The functions common to all timers
(`start()`, `stop()`, `end()`, `setPrescaler()`, the getters, copying and moving) are described in
[Common functions](common.md).

---

### PeriodicTimer()

```cpp
PeriodicTimer(TimerGenerator* timerType = nullptr);
```

Creates a timer object. No hardware is allocated yet: `begin()` takes a channel of the given timer type,
or, without a timer type, the next free timer from the [timer pool](../configuration.md#timer-pool).

| Parameter | |
|---|---|
| `timerType` | the timer type, e.g. `PIT`, `TMR1`, `TCK`; see [Timer types](timer-types.md). `nullptr` (default): timer pool |

```cpp
PeriodicTimer t1;      // timer pool
PeriodicTimer t2(PIT); // a PIT channel
```

---

### begin()

```cpp
template <typename period_t>
errorCode begin(callback_t callback, period_t period, bool start = true);
```

Allocates a timer channel (on the first call), sets the callback and the period and starts the timer.
Called on a running timer, `begin()` restarts it with the new callback and period; the previous callback
is no longer invoked.

| Parameter | |
|---|---|
| `callback` | the function to invoke, see [Callbacks](../callbacks.md) |
| `period` | the period, in microseconds or as `std::chrono` duration. Rounded to the nearest number of timer ticks, at least one tick |
| `start` | `true` (default): start the timer; `false`: only configure it, `start()` starts it |

| Return value | Condition |
|---|---|
| `OK` | the timer runs (or is configured, with `start = false`) |
| `periodOverflow` (warning) | `period` is longer than `getMaxPeriod()`; the timer runs with the maximum period |
| `prescalerIgnored` (warning) | `setPrescaler()` was called before `begin()` on a timer type without prescaler; the timer runs |
| `argument` | `period` is negative; or `setPrescaler()` was called before with an invalid value, in this case the timer runs with the default prescaler |
| `callback` | `callback` is empty (`nullptr`) |
| `reload` | `period` is 0 |
| `noFreeChannel` | all channels of the requested timer type are in use |
| `noFreeModule` | timer pool: no free timer left |

Except where noted, the timer is unchanged after an error; a running timer keeps its callback and period.

**Supported timers:** all.

<!-- tests: api_periodic_period, api_chrono_durations, api_begin_stopped, api_begin_running_timer,
     api_max_period_and_overflow, api_period_zero, api_negative_values, api_error_func, api_no_free_channel,
     api_pool_exhausted, api_set_prescaler -->

---

### setPeriod()

```cpp
template <typename period_t>
errorCode setPeriod(period_t period, PeriodMode mode = PeriodMode::restart);
```

Changes the period of the timer.

- `PeriodMode::restart` (default): the new period starts at the call; the next callback occurs one new
  period after the call.
- `PeriodMode::keepElapsed`: the time elapsed in the current period counts towards the new period. If it
  already exceeds the new period, the callback occurs immediately. The following periods have the new
  length.

A stopped timer stays stopped; `start()` starts it with the new period.

| Parameter | |
|---|---|
| `period` | the new period, in microseconds or as `std::chrono` duration. Rounded to the nearest number of timer ticks, at least one tick |
| `mode` | `PeriodMode::restart` (default) or `PeriodMode::keepElapsed` |

| Return value | Condition |
|---|---|
| `OK` | the new period is set |
| `periodOverflow` (warning) | `period` is longer than `getMaxPeriod()`; the maximum period is used |
| `argument` | `period` is negative; the timer is unchanged |
| `reload` | `period` is 0; the timer is unchanged |
| `notInitialized` | called before `begin()` |
| `notImplemented` | the timer doesn't support `setPeriod()` or the given mode; the timer is unchanged |

**Supported timers:** `restart`: all Teensy 4 timers and the TCK timers. `keepElapsed`: PIT, TMR1 … TMR4,
TCK, TCK64, TCK_RTC. Not supported: FTM0 … FTM3 (Teensy 3); `keepElapsed` on GPT1, GPT2, RTC.

**Notes**

- The RTC timer synchronizes its periods with the clock: after `setPeriod()`, the next callback occurs
  within one period, not exactly one period after the call.
- **Changed in v1.5:** up to v1.4, `setPeriod()` of the TMR and TCK timers behaved like
  `PeriodMode::keepElapsed`.

**See also:** [`setNextPeriod()`](#setnextperiod), [Changing the period](../usage.md#changing-the-period).

<!-- tests: api_set_period, api_set_period_restarts, api_set_period_keep_elapsed, api_period_zero,
     api_negative_values, api_set_period_before_begin, api_max_period_and_overflow
     open finding (PLAN_API_v1.5.md): PIT setPeriod() currently starts a stopped timer, fix before release -->

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
| `period` | the new period, in microseconds or as `std::chrono` duration. Rounded to the nearest number of timer ticks, at least one tick |

| Return value | Condition |
|---|---|
| `OK` | the new period is set |
| `periodOverflow` (warning) | `period` is longer than `getMaxPeriod()`; the maximum period is used |
| `argument` | `period` is negative; the timer is unchanged |
| `reload` | `period` is 0; the timer is unchanged |
| `notInitialized` | called before `begin()` |
| `notImplemented` | the timer doesn't support `setNextPeriod()`; the timer is unchanged |

**Supported timers:** PIT, TMR1 … TMR4, TCK, TCK64, TCK_RTC.
Not supported: GPT1, GPT2, RTC (Teensy 4), FTM0 … FTM3 (Teensy 3).

**Notes**

- `getPeriod()` returns the new period immediately after the call, i.e. the latest period set, not the
  period currently running.
- Intended for running timers. To change the period of a stopped timer, use `setPeriod()`.
- `setNextPeriod()` can be called from the timer's own callback.

**See also:** [`setPeriod()`](#setperiod), [Changing the period at the end of the current period](../usage.md#changing-the-period-at-the-end-of-the-current-period)
(example: alternating periods).

<!-- tests: api_set_next_period, la_next_period, api_period_zero, api_negative_values,
     api_set_period_before_begin, api_get_period, api_error_posted_once, api_max_period_and_overflow
     open finding (PLAN_API_v1.5.md): behaviour on a stopped timer differs between PIT and TMR/TCK -->

---

### getPeriod()

```cpp
template <typename T = float>
T getPeriod() const;
```

Returns the period set by the latest `begin()`, `setPeriod()` or `setNextPeriod()`, rounded as the
timer uses it: e.g. 99.84 µs for a TMR timer started with 100 µs, the maximum period after an overflow.
After `setPrescaler()` on a running timer, it returns the period resulting from the new prescaler.

| Template parameter | |
|---|---|
| `T` | `float` (default) or `double`: seconds; or a `std::chrono` duration, e.g. `std::chrono::microseconds`. Integer durations are rounded to the nearest unit |

| Return value | Condition |
|---|---|
| the period | |
| `NAN` / `T::zero()`, reports `notInitialized` | called before `begin()` |
| `NAN` / `T::zero()`, reports `notImplemented` | the timer doesn't support `getPeriod()` |

**Supported timers:** all Teensy 4 timers, TCK, TCK64. Not supported: FTM0 … FTM3 (Teensy 3).

<!-- tests: api_get_period, api_chrono_getters, api_queries_before_begin, la_periodic, la_prescaler -->
