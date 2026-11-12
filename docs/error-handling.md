# Error handling

Most functions of TeensyTimerTool return an `errorCode`. It tells whether the function succeeded, did its
job with a restriction (*warning*) or could not do its job (*error*). Typical reasons are a period
longer than the timer supports, a timer type without free channels, or a function the timer doesn't
support. TeensyTimerTool doesn't stop the sketch in these cases; it reports them, and the sketch decides
what to do.

There are three ways to handle the reported codes:

1. check the returned code in the sketch,
2. attach an error function which is called automatically,
3. use the built-in error handler, which prints the codes to a serial port.

## Checking the returned code

`errorCode::OK` means success. The other values can be compared with the named codes:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t1(TCK);

void setup()
{
    while (!Serial) {}
    pinMode(LED_BUILTIN, OUTPUT);

    errorCode err = t1.begin([] { digitalToggleFast(LED_BUILTIN); }, 10s);
    if (err == errorCode::periodOverflow)
    {
        Serial.printf("10 s is too long, the timer uses %.1f s\n", t1.getMaxPeriod());
    }
    else if (err != errorCode::OK)
    {
        Serial.printf("begin() failed: %d\n", (int)err);
    }
}

void loop()
{
}
```

The TCK timer supports periods of up to 5 s (Teensy 3 and 4). `begin()` returns the warning `periodOverflow`, and the
timer runs with its maximum period.

## Attaching an error function

Checking every call is tedious. With `attachErrFunc()`, the library calls a function of the sketch for
every result other than `OK`, from any function of the library:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

OneShotTimer t1;

void onError(errorCode err)
{
    Serial.printf("TeensyTimerTool reported %d\n", (int)err);
}

void setup()
{
    while (!Serial) {}
    attachErrFunc(onError);

    t1.trigger(10ms); // error: trigger() before begin() -> notInitialized (106)
}

void loop()
{
}
```

The error function is called exactly once per failed call. It can also be a lambda expression. The
returned code is still available to the sketch.

## The built-in error handler

The library provides a ready-made error handler, which prints the code and a short description to a
serial port. It is attached like any other error function:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t1(TCK);
OneShotTimer t2;

void setup()
{
    while (!Serial) {}
    attachErrFunc(ErrorHandler(Serial)); // print warnings and errors on Serial

    t1.begin([] {}, 10s); // warning: a TCK timer supports up to 5 s
    t2.trigger(10ms);     // error: trigger() before begin()
}

void loop()
{
}
```

Output:

```
W-100: Period overflow. Period was set to maximum value
E-106: Timer not initialized or available. Did you call begin?
```

- For a **warning** (`W-…`), the handler prints the message and returns; the sketch continues.
- For an **error** (`E-…`), it prints the message and stops the sketch: it blinks the built-in LED in a
  fast, endless loop.

!!! warning "LED_BUILTIN"
    The built-in error handler sets `LED_BUILTIN` to output when it is created and uses it to signal
    errors. Don't use it if something else is connected to this pin.

## Error codes

| Code | Value | Meaning |
|---|---|---|
| `OK` | 0 | success |
| **Warnings** | | the function did its job with a restriction |
| `periodOverflow` | -100 | the period or delay is longer than the timer supports; the maximum is used |
| `prescalerIgnored` | -103 | `setPrescaler()` was called before `begin()` on a timer without prescaler; the timer runs without |
| **Errors** | | the function could not do its job |
| `argument` | 100 | invalid argument, e.g. a negative period or an unsupported prescaler value |
| `callback` | 101 | `begin()` without a valid callback (`nullptr`) |
| `reload` | 102 | period 0 |
| `noFreeModule` | 103 | the timer pool has no free timer left |
| `noFreeChannel` | 104 | the requested timer type has no free channel left |
| `notImplemented` | 105 | the timer doesn't support this function |
| `notInitialized` | 106 | the function needs a timer, but `begin()` wasn't called (or failed) |

Which function returns which code under which condition is listed for every function in the
[Reference](reference/index.md).

---

## Details

- After an error, the timer is unchanged: a running timer keeps running with its previous settings.
  After a warning, the timer runs with the restriction described above.
- The getters (`getPeriod()`, `getMaxPeriod()` …) return a value instead of an `errorCode`. If they
  fail, they return `NAN` (or `zero()` for `std::chrono` durations) and report the code to the error
  function.
- `attachErrFunc()` replaces a previously attached function. `attachErrFunc(nullptr)` removes it.
- The codes `wrongType` (-101) and `triggeredLate` (-102) are currently not used. Values of 200 and
  above are reserved for internal errors of the timer modules.
