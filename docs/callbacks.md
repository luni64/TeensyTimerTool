# Callbacks

The *callback* is the function a timer invokes when its period or delay has expired. TeensyTimerTool
accepts all kinds of callable objects as callback: plain functions, lambda expressions, functors and,
through a lambda, member functions of a class. This chapter shows the common cases.

## Plain functions

Any function without parameters and without return value can be used as callback:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t1;

void toggleLED()
{
    digitalToggleFast(LED_BUILTIN);
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    t1.begin(toggleLED, 250ms);
}

void loop()
{
}
```

### Callbacks and the rest of the sketch

A callback and `loop()` often share data, e.g. a counter incremented by the callback and printed in
`loop()`. Such variables must be declared `volatile`, otherwise the compiler may assume that they don't
change while `loop()` runs:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t1;
volatile unsigned counter = 0; // changed by the callback, read by loop()

void setup()
{
    t1.begin([] { counter++; }, 1ms);
}

void loop()
{
    Serial.println(counter); // about 1000 more every second
    delay(1000);
}
```

Callbacks of hardware timers run in interrupt context (1) and should do their work quickly and return.
Long operations, `delay()` or printing large amounts of text in a callback delay the rest of the sketch
and other interrupts.
{ .annotate }

1.  **Interrupt context**

    The callbacks of the hardware timers are invoked from an interrupt handler: they interrupt the
    sketch at an arbitrary point. Keep them short, and declare variables that are changed in a callback
    and read in `loop()` as `volatile`. The callbacks of the TCK timers run in `yield()` instead, i.e.
    between two `loop()` iterations or during `delay()`.

## Lambda expressions

For short callbacks, a separate function is not needed. A lambda expression (1) defines the callback
directly in the call to `begin()`:
{ .annotate }

1.  **Lambda expression**

    A *lambda expression* is a function without a name, written directly where it is needed:
    `[] { digitalToggleFast(13); }`. The square brackets can list variables the function should
    *capture*, i.e. take along. [Introduction to lambdas](http://sandordargo.com/blog/2018/12/19/c++-lambda-expressions)

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

OneShotTimer t1;

void setup()
{
    while (!Serial) {}
    t1.begin([] { Serial.printf("called at %lu ms\n", millis()); });

    Serial.printf("triggered at %lu ms\n", millis());
    t1.trigger(100ms);
}

void loop()
{
}
```

Output:

```
triggered at 384 ms
called at 484 ms
```

## Callbacks with parameters

A callback is invoked without arguments. To call a function that needs parameters, wrap the call in a
lambda expression. In the following sketch, three one-shot timers call the same function, each with a
different pin number:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

OneShotTimer t1, t2, t3;
elapsedMillis stopwatch;

void handlePin(unsigned pin)
{
    unsigned ms = stopwatch;
    Serial.printf("pin %u (%u ms)\n", pin, ms);
}

void setup()
{
    while (!Serial) {}

    t1.begin([] { handlePin(1); });
    t2.begin([] { handlePin(2); });
    t3.begin([] { handlePin(3); });

    stopwatch = 0;
    t1.trigger(20ms);
    t2.trigger(5ms);
    t3.trigger(50ms);
}

void loop()
{
}
```

Output:

```
pin 2 (5 ms)
pin 1 (20 ms)
pin 3 (50 ms)
```

Values that are only known at runtime are *captured* by the lambda. The following sketch starts ten
timers in a loop; each lambda captures its own value of `i`:

```cpp
--8<-- "02_Advanced/CapturingLambdas/CapturingLambdas.ino"
```

!!! warning "Capture by value"
    Capture variables by value, as `[i]` above. A capture by reference (`[&i]`) refers to the variable
    itself, which may no longer exist or may have changed when the callback is invoked.

## Member functions

A member function needs an object to be called on, so it can't be passed to `begin()` directly. A lambda
expression calls it on a given object:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

class Blinker
{
 public:
    Blinker(unsigned pin) : pin(pin) {}

    void begin() { pinMode(pin, OUTPUT); }
    void blink() { digitalToggleFast(pin); }

 protected:
    unsigned pin;
};

Blinker b1(1), b2(LED_BUILTIN);
PeriodicTimer t1, t2;

void setup()
{
    b1.begin();
    b2.begin();
    t1.begin([] { b1.blink(); }, 1ms);
    t2.begin([] { b2.blink(); }, 250ms);
}

void loop()
{
}
```

## Timers inside a class

In the example above, the timers are only needed to call `blink()`. It is cleaner to make the timer a
member of the class: the class then hides the timer completely, and the sketch only creates `Blinker`
objects. The lambda captures `this`, the pointer to the object, and calls the member function on it:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

class Blinker
{
 public:
    Blinker(unsigned pin, float periodMs) : pin(pin), periodMs(periodMs) {}

    void begin() // don't initialize the hardware in the constructor
    {
        pinMode(pin, OUTPUT);
        timer.begin([this] { blink(); }, periodMs * 1000);
    }

 protected:
    void blink() { digitalToggleFast(pin); }

    unsigned pin;
    float periodMs;
    PeriodicTimer timer;
};

Blinker b1(1, 1), b2(LED_BUILTIN, 250); // pin, period in ms

void setup()
{
    b1.begin();
    b2.begin();
}

void loop()
{
}
```

The sketch doesn't need to know anything about timers or lambdas; it just creates and starts
`Blinker` objects.

## Functors

A *functor* is an object of a class with a function call operator `operator()`. It can store data, e.g.
settings, and is used as callback like a function. The following functor generates a pulse of
adjustable length:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

class PulseGenerator
{
 public:
    PulseGenerator(unsigned pin, unsigned widthUs) : pin(pin), widthUs(widthUs) {}

    void operator()() // invoked by the timer
    {
        digitalWriteFast(pin, HIGH);
        delayMicroseconds(widthUs);
        digitalWriteFast(pin, LOW);
    }

 protected:
    unsigned pin, widthUs;
};

OneShotTimer t1, t2;

void setup()
{
    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT);
    t1.begin(PulseGenerator(1, 5));  //  5 µs pulse on pin 1
    t2.begin(PulseGenerator(2, 10)); // 10 µs pulse on pin 2
}

void loop()
{
    t1.trigger(1ms);
    t2.trigger(500us);
    delay(10);
}
```

## Arrays and containers of timers

Timers can be kept in arrays, which is convenient for a number of identical tasks:

```cpp
--8<-- "01_Basic/ArrayOfTimers/ArrayOfTimers.ino"
```

Timers can also be stored in standard containers like `std::vector`, and added or removed at runtime.
Timers can't be copied, but they can be moved: when a vector grows or an element is erased, the running
timers are moved and continue to run. A timer that is removed from the container stops and releases its
channel. The example `TimersInContainers` shows this with a set of blinking pins controlled from the
serial monitor:

```cpp
--8<-- "02_Advanced/TimersInContainers/TimersInContainers.ino"
```

---

## Details

### The callback type

By default, a callback is stored as `stdext::inplace_function<void()>`, a variant of `std::function`
that never allocates memory on the heap. A lambda expression can capture up to 32 bytes, e.g. eight
`int` values or pointers; a larger capture is rejected by the compiler. To pass more data, capture a
pointer or a reference to a structure that lives long enough.

With the configuration option `PLAIN_VANILLA_CALLBACKS`, callbacks are plain function pointers
(`void (*)()`). Plain functions and lambda expressions without captures still work, capturing lambdas
and functors don't compile. See [Configuration](configuration.md).

### Capturing `this` and moving objects

A lambda that captures `this` stores the address of the object. If the object is moved, e.g. a `Blinker`
from the example above stored in a `std::vector` that reallocates, the timer moves along, but its
callback still refers to the old address. Classes whose timer callback captures `this` should therefore
not be moved: create them where they stay (globally or with `new`), or delete their move constructor and
move assignment.
