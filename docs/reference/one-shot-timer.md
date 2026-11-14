# OneShotTimer

```cpp
class OneShotTimer; // namespace TeensyTimerTool
```

A `OneShotTimer` invokes its callback once, after the delay passed to `trigger()`. The functions common to
all timers (`stop()`, `end()`, `setPrescaler()`, the getters, copying and moving) are described in
[Common functions](common.md).

---

### OneShotTimer()

```cpp
OneShotTimer(TimerGenerator* timerType = nullptr);
```

Creates a timer object. No hardware is allocated yet: `begin()` takes a channel of the given timer type,
or, without a timer type, the next free timer from the [timer pool](../configuration.md#timer-pool).

| Parameter | |
|---|---|
| `timerType` | the timer type, e.g. `TMR1`, `GPT1`, `TCK`; see [Timer types](timer-types.md). `nullptr` (default): timer pool |

```cpp
OneShotTimer t1;       // timer pool
OneShotTimer t2(TMR1); // a channel of TMR1
```

---

### begin()

```cpp
errorCode begin(callback_t callback);
```

Allocates a timer channel (on the first call) and sets the callback. The timer is not started; the
callback is not invoked until `trigger()` is called.

```cpp
t2.begin(onTimer);
t2.begin([] { digitalWriteFast(LED_BUILTIN, LOW); }); // lambda expression as callback
```

| Parameter | |
|---|---|
| `callback` | the function to invoke, see [Callbacks](../callbacks.md) |

| Return value | Condition |
|---|---|
| `OK` | the timer is ready |
| `prescalerIgnored` (warning) | `setPrescaler()` was called before `begin()` on a timer type without prescaler |
| `invalidArgument` | `setPrescaler()` was called before with an invalid value; the default prescaler is used |
| `noCallback` | `callback` is empty (`nullptr`) |
| `noFreeChannel` | all channels of the requested timer type are in use |
| `poolExhausted` | timer pool: no free timer left |
| `notSupported` | the timer type doesn't support one-shot use (RTC) |

**Supported timers:** all except RTC.

<!-- tests: api_one_shot, api_error_func, api_no_free_channel -->

---

### trigger()

```cpp
template <typename period_t>
errorCode trigger(period_t delay);
```

Starts the timer: the callback is invoked once, `delay` after the call. `trigger()` returns immediately.
Called while a trigger is pending, it restarts the delay; the callback is invoked once. Called from the
callback, it re-triggers the timer, which allows chaining delays.

`trigger()` is a template so that the delay can be passed in different types: as integer or floating
point number (microseconds) or as `std::chrono` duration. The compiler deduces `period_t` from the
argument.

```cpp
t2.trigger(500);   // callback in 500 µs
t2.trigger(1.5s);  // callback in 1.5 s
```

| Parameter | |
|---|---|
| `delay` | the delay, in microseconds or as `std::chrono` duration. Rounded to the nearest number of timer ticks; `0` invokes the callback after one tick |

| Return value | Condition |
|---|---|
| `OK` | the timer runs |
| `periodOverflow` (warning) | `delay` is longer than `getMaxPeriod()`; the maximum delay is used |
| `invalidArgument` | `delay` is negative; no callback |
| `notInitialized` | called before `begin()` |

**Supported timers:** all except RTC.

**Notes**

- The callback starts about 0.3 … 1.1 µs later than `delay` after the call, depending on the timer; see
  [Performance](../advanced/performance.md). The TCK timers are delayed further until `yield()` runs.
- `TCK_RTC` counts whole ticks of the 32.768 kHz clock, and the trigger occurs somewhere within a tick:
  the delay can be up to one tick (30.5 µs) shorter than requested.

**See also:** [`triggerDirect()`](#triggerdirect), [`stop()`](common.md#stop).

<!-- tests: api_one_shot, api_retrigger, api_trigger_from_callback, api_trigger_zero, api_negative_values,
     api_trigger_before_begin, la_one_shot -->

---

### triggerDirect()

```cpp
template <typename T>
errorCode triggerDirect(T reload);
```

Like `trigger()`, but the delay is given in timer ticks, as returned by `getTriggerReload()`. Saves the
conversion of the delay, e.g. for timers that are re-triggered with the same delay many times. The
template parameter `T` is the type of the reload value; the compiler deduces it from the argument.

```cpp
uint32_t reload;
t2.getTriggerReload(20, &reload); // 20 µs in timer ticks, calculated once
t2.triggerDirect(reload);         // callback in 20 µs
```

| Parameter | |
|---|---|
| `reload` | the delay in timer ticks, from `getTriggerReload()`: `uint32_t`; `uint64_t` for TCK64 and TCK_RTC |

| Return value | Condition |
|---|---|
| `OK` | the timer runs |
| `periodOverflow` (warning) | `reload` exceeds the counter of the timer (TMR: 0xFFFE); the maximum is used |
| `notInitialized` | called before `begin()` |
| `notSupported` | the timer doesn't support `triggerDirect()` with this type of `reload` |

**Supported timers:** PIT, GPT1, GPT2, TMR1 … TMR4, TCK (`uint32_t`), TCK64, TCK_RTC (`uint64_t`), FTM0 … FTM3.

**See also:** [Precomputed reload values](../advanced/performance.md#precomputed-reload-values) (example).

<!-- tests: api_trigger_direct, perf_*_one_shot -->

---

### getTriggerReload()

```cpp
template <typename T>
errorCode getTriggerReload(float delay, T* reload);
```

Converts a delay into the reload value for `triggerDirect()`, with the same rounding as `trigger()`.

```cpp
uint32_t reload;                  // uint64_t for TCK64 and TCK_RTC
t2.getTriggerReload(20, &reload); // 20 µs in timer ticks
```

| Parameter | |
|---|---|
| `delay` | the delay in microseconds |
| `reload` | receives the reload value: `uint32_t`; `uint64_t` for TCK64 and TCK_RTC |

| Return value | Condition |
|---|---|
| `OK` | `*reload` is set |
| `periodOverflow` (warning) | `delay` is longer than `getMaxPeriod()`; `*reload` is the maximum |
| `notInitialized` | called before `begin()` |
| `notSupported` | the timer doesn't support it with this type of `reload` |

**Supported timers:** as `triggerDirect()`.

<!-- tests: api_trigger_direct -->

---

### start()

```cpp
errorCode start();
```

Has no function for one-shot timers; use `trigger()`. Always returns `notSupported`; the timer is not
changed.

<!-- tests: api_one_shot_start -->
