# Common functions

The following functions are available for `PeriodicTimer` and `OneShotTimer`. `begin()` differs between
the two classes; see [PeriodicTimer](periodic-timer.md#begin) and [OneShotTimer](one-shot-timer.md#begin).

---

### start()

```cpp
errorCode start();
```

Starts a stopped periodic timer, or a timer configured with `begin(callback, period, false)`. The period
starts from the beginning. For one-shot timers, see [`OneShotTimer::start()`](one-shot-timer.md#start).

```cpp
t1.begin(onTimer, 100ms, false); // configured, not running
t1.start();                      // first callback 100 ms from now
```

| Return value | Condition |
|---|---|
| `OK` | the timer runs |
| `notInitialized` | called before `begin()` |

**Supported timers:** all.

<!-- tests: api_begin_stopped, api_stop_start -->

---

### stop()

```cpp
errorCode stop();
```

Stops the timer: no further callbacks are invoked. On a one-shot timer, a pending trigger is cancelled.
The timer keeps its channel and settings; `start()` (periodic) or `trigger()` (one-shot) use it again.

```cpp
t1.stop(); // no more callbacks until start()
t2.stop(); // cancels a pending trigger of the one-shot timer
```

| Return value | Condition |
|---|---|
| `OK` | the timer is stopped, also if it wasn't running |
| `notInitialized` | called before `begin()` |

**Supported timers:** all.

<!-- tests: api_stop_start, api_one_shot_stop, la_one_shot_stop -->

---

### end()

```cpp
errorCode end();
```

Stops the timer and releases its channel. The channel is free for other timers; a later `begin()`
allocates a new channel.

```cpp
t1.end();                // the channel is free for other timers
t1.begin(onTimer, 10ms); // takes a channel again
```

| Return value | Condition |
|---|---|
| `OK` | always, also if the timer had no channel |

**Supported timers:** all.

<!-- tests: api_end -->

---

### setPrescaler()

```cpp
errorCode setPrescaler(int prescaler);
```

Sets the prescaler, which divides the timer clock by a power of two (see
[Prescaler](../choosing-a-timer.md#prescaler)). Called before `begin()`, the value is stored and
applied when `begin()` allocates the channel; `begin()` reports an invalid value or a timer without
prescaler. Called on a running timer, the new prescaler takes effect immediately: the period keeps its
number of ticks and therefore changes with the prescaler, until the next `setPeriod()` or
`setNextPeriod()`.

```cpp
PeriodicTimer t1(GPT1);
t1.setPrescaler(PSC_4096); // maximum period 8.5 days
t1.begin(onTimer, 24h);
```

| Parameter | |
|---|---|
| `prescaler` | `PSC_1`, `PSC_2`, `PSC_4` … `PSC_128` (TMR), … `PSC_4096` (GPT) |

| Return value | Condition |
|---|---|
| `OK` | the prescaler is set; before `begin()`: the value is stored |
| `argument` | invalid value for this timer, e.g. `PSC_256` on a TMR timer or `PSC_AUTO`; the prescaler is unchanged |
| `notImplemented` | the timer has no prescaler |

**Supported timers:** TMR1 … TMR4 (`PSC_1` … `PSC_128`), GPT1, GPT2 (`PSC_1` … `PSC_4096`).

<!-- tests: api_set_prescaler, la_prescaler, api_error_posted_once -->

---

### Getters with units

The getters described below (`getMaxPeriod()`, `getResolution()`, `getRemainingTime()`) and
[`getPeriod()`](periodic-timer.md#getperiod) take an optional template parameter, which selects the
unit of the result. Without it, they return seconds as `float`; with a `std::chrono` duration, they
return the value in this unit:

```cpp
float s = t1.getPeriod();                           // seconds, e.g. 0.00009984
auto  p = t1.getPeriod<std::chrono::microseconds>(); // e.g. 100 µs for 99.84 µs
auto  r = t1.getRemainingTime<std::chrono::duration<double, std::micro>>(); // µs with fraction
```

Integer durations are rounded to the nearest unit; values too large for the duration type return
`T::max()`. On errors, the getters return `T::zero()` and report the error. Requires the configuration
option `USE_TIME_LITERALS` (default).

---

### getMaxPeriod()

```cpp
template <typename T = float>
T getMaxPeriod() const;
```

Returns the longest period or delay the timer supports, with its current prescaler.

```cpp
float maxSeconds = t1.getMaxPeriod(); // e.g. 178.96 for a GPT timer
```

| Template parameter | |
|---|---|
| `T` | `float` (default) or `double`: seconds; or a `std::chrono` duration. Values too large for the duration type return `T::max()` |

| Return value | Condition |
|---|---|
| the maximum period | |
| `NAN` / `T::zero()`, reports `notInitialized` | called before `begin()` |

**Supported timers:** all.

<!-- tests: api_max_period_and_overflow, api_chrono_getters, api_queries_before_begin -->

---

### getResolution()

```cpp
template <typename T = float>
T getResolution() const;
```

Returns the duration of one timer tick with the current prescaler, e.g. 41.7 ns for the PIT, 853 ns for a
TMR timer with `PSC_128`, 30.5 µs for TCK_RTC and RTC.

```cpp
float tick = t1.getResolution(); // seconds, e.g. 0.0000000417 for the PIT
```

| Return value | Condition |
|---|---|
| the tick period | |
| `NAN` / `T::zero()`, reports `notInitialized` | called before `begin()` |
| `NAN` / `T::zero()`, reports `notImplemented` | the timer doesn't support it |

**Supported timers:** all Teensy 4 timers, TCK, TCK64. Not supported: FTM0 … FTM3 (Teensy 3).

<!-- tests: api_get_period, api_chrono_getters -->

---

### getRemainingTime()

```cpp
template <typename T = float>
T getRemainingTime() const;
```

Returns the time until the next callback. Returns 0 if no callback is pending: the timer is stopped, or
a one-shot timer was never triggered, has fired or was stopped.

```cpp
float remaining = t2.getRemainingTime(); // seconds until the callback of the one-shot timer
```

| Return value | Condition |
|---|---|
| the remaining time | |
| `NAN` / `T::zero()`, reports `notInitialized` | called before `begin()` |
| `NAN` / `T::zero()`, reports `notImplemented` | the timer doesn't support it |

**Supported timers:** PIT, GPT1, GPT2, TMR1 … TMR4, TCK, TCK64, TCK_RTC. Not supported: RTC, FTM0 … FTM3.

<!-- tests: api_remaining_time, api_remaining_time_idle -->

---

### Copying, moving and destruction

- **Copying** a timer doesn't compile: two objects would control the same channel.
- **Moving** is possible, e.g. into a `std::vector`. The new object takes over the channel, the timer
  keeps running. The moved-from object is empty: `start()`, `stop()`, `trigger()` return
  `notInitialized`, `begin()` allocates a new channel. Move assignment releases the channel the target
  owned before.
- **Destroying** a timer object stops the timer and releases its channel, like `end()`.

<!-- tests: api_not_copyable, api_move, api_timers_in_vector, api_destruction_frees_channel -->
