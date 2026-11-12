# Errors and other functions

## errorCode

```cpp
enum class errorCode; // namespace TeensyTimerTool
```

The result of most functions. `OK` (0) is success, negative values are warnings (the function did its
job with a restriction), positive values are errors (the function could not do its job).

| Code | Value | Meaning | Returned by |
|---|---|---|---|
| `OK` | 0 | success | all |
| `periodOverflow` | -100 | period or delay too long, the maximum is used | `begin()`, `setPeriod()`, `setNextPeriod()`, `trigger()`, `triggerDirect()`, `getTriggerReload()` |
| `prescalerIgnored` | -103 | `setPrescaler()` before `begin()` on a timer without prescaler | `begin()` |
| `argument` | 100 | invalid argument (negative period or delay, invalid prescaler) | `begin()`, `setPeriod()`, `setNextPeriod()`, `trigger()`, `setPrescaler()` |
| `callback` | 101 | empty callback (`nullptr`) | `begin()` |
| `reload` | 102 | period 0 | `begin()` (periodic), `setPeriod()`, `setNextPeriod()` |
| `noFreeModule` | 103 | the timer pool has no free timer | `begin()` |
| `noFreeChannel` | 104 | the requested timer type has no free channel | `begin()` |
| `notImplemented` | 105 | the timer doesn't support the function | see [Timer types](timer-types.md#supported-functions) |
| `notInitialized` | 106 | called before `begin()` | all functions except `begin()`, `end()`, `setPrescaler()`, `OneShotTimer::start()` |

`wrongType` (-101) and `triggeredLate` (-102) are currently not used; values of 200 and above are reserved
for internal errors of the timer modules.

---

### attachErrFunc()

```cpp
void attachErrFunc(errorFunc_t errorFunction);
```

Sets a function which is called for every result other than `OK`, exactly once per failed call, also for
getters that return `NAN`. Replaces a previously attached function; `nullptr` removes it.

```cpp
attachErrFunc([](errorCode e) { Serial.printf("TeensyTimerTool reported %d\n", (int)e); });
```

| Parameter | |
|---|---|
| `errorFunction` | a function, lambda or functor with the signature `void(errorCode)`, or `nullptr` |

**See also:** [Error handling](../error-handling.md).

<!-- tests: api_error_func, api_error_posted_once -->

---

### ErrorHandler

```cpp
class ErrorHandler
{
 public:
    ErrorHandler(Stream& stream);
    void operator()(errorCode code) const;
};
```

A ready-made error function for `attachErrFunc()`: `attachErrFunc(ErrorHandler(Serial));`

- Warnings are printed as `W-<code>: <text>`; the sketch continues.
- Errors are printed as `E-<code>: <text>`; the handler then blinks `LED_BUILTIN` in an endless loop.

The constructor sets `LED_BUILTIN` to output.

---

### tick()

```cpp
void tick(); // namespace TeensyTimerTool
```

Evaluates the TCK timers and invokes the callbacks that are due. With the default configuration, the
library calls `tick()` from `yield()`; a sketch only calls it itself with the configuration option
`YIELD_NONE`, then as often as possible.

```cpp
void loop()
{
    TeensyTimerTool::tick(); // only with YIELD_NONE
    // ...
}
```

---

### PeriodMode

```cpp
enum class PeriodMode { restart, keepElapsed }; // namespace TeensyTimerTool
```

The second parameter of [`setPeriod()`](periodic-timer.md#setperiod).

---

### Prescaler constants

```cpp
PSC_1, PSC_2, PSC_4, PSC_8, PSC_16, PSC_32, PSC_64, PSC_128           // TMR and GPT
PSC_256, PSC_512, PSC_1024, PSC_2048, PSC_4096                        // GPT only
PSC_AUTO                                                              // FTM_DEFAULT_PSC only (configuration)
```

The parameter of [`setPrescaler()`](common.md#setprescaler) and the prescaler settings of the
[configuration](../configuration.md).
