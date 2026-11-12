# Callbacks

The *callback* is the function a timer invokes when its period or delay has expired. In contrast to the
classic Arduino callbacks, which are plain functions, TeensyTimerTool accepts all kinds of callable
objects as callback: plain functions, lambda expressions and functors. Lambda expressions and functors
can carry data, which allows callbacks with parameters and callbacks that call a member function of an
object. This chapter shows the common cases.

## Plain functions

This is the standard Arduino way: any function without parameters and without return value can be used
as callback.

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

A plain function is sufficient as long as the callback always does the same job. It gets tedious when
the callback needs information from the sketch, its *state*: which pin to toggle, which object to
update, which value to use. A plain callback has no parameters, so this information has to be stored in
global variables or written into the function itself. Three timers toggling three pins need three
nearly identical functions:

```cpp
void togglePin1() { digitalToggleFast(1); }
void togglePin2() { digitalToggleFast(2); }
void togglePin3() { digitalToggleFast(3); }
```

Lambda expressions and functors avoid this: they are callbacks that carry their state with them. With
them, one function serves any number of timers (see [Callbacks with parameters](#callbacks-with-parameters)),
and a timer can call a member function of a specific object (see [Member functions](#member-functions)).

## Lambda expressions

A lambda expression (1) defines the callback directly in the call to `begin()`. For short callbacks, a
separate function is not needed:
{ .annotate }

1.  **Lambda expression**

    A *lambda expression* is a function without a name, written directly where it is needed:
    `[] { digitalToggleFast(13); }`. The square brackets can list variables the function should
    *capture*, i.e. take along. [Introduction to lambdas](http://sandordargo.com/blog/2018/12/19/c++-lambda-expressions)

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t1;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    t1.begin([] { digitalToggleFast(LED_BUILTIN); }, 250ms);
}

void loop()
{
}
```

### Callbacks with parameters

Plain callbacks can only be invoked without arguments. A lambda expression, however, can call any
function with any arguments. In the following sketch, three one-shot timers call the same function,
each with a different pin number:

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
    while (!Serial) {} // wait for the serial monitor, otherwise the output is lost

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

### Capturing variables

Values that are only known at runtime can be *captured* by the lambda. The following sketch starts ten
timers in a loop; each lambda captures its own value of `i`:

```cpp
--8<-- "02_Advanced/CapturingLambdas/CapturingLambdas.ino"
```

The square brackets of a lambda expression list the variables it captures. There are two ways to
capture a variable:

- **By value**, `[i]`: the lambda stores a copy of `i`, taken when the lambda is created, here in the
  call to `begin()`. The callback always works with this copy, whatever happens to `i` afterwards. In
  the sketch above, each of the ten lambdas stores its own number.
- **By reference**, `[&i]`: the lambda stores only a reference to the variable. The callback works with
  the variable itself: it sees the value the variable has at the time the callback is invoked, and it
  can change it. The variable must therefore still exist when the callback is invoked, which can be
  long after `begin()` has returned.

```cpp
void setup()
{
    int n = 42;

    t1.begin([n]  { Serial.println(n); }, 1s); // prints 42 every second: the lambda has its own copy
    t2.begin([&n] { Serial.println(n); }, 1s); // wrong: n no longer exists when setup() has returned
}
```

Global variables don't need to be captured; a lambda can use them directly, like any function. This
leaves few use cases for a capture by reference in a timer callback. One of them is a callback that has
to change a variable which is selected at runtime. In the following sketch, one function starts any
number of timers, each counting in its own variable:

```cpp
volatile unsigned fastCount = 0, slowCount = 0; // global: they exist as long as the sketch runs

void startCounter(PeriodicTimer& timer, volatile unsigned& counter, float periodUs)
{
    timer.begin([&counter] { counter++; }, periodUs); // the callback increments the variable passed in
}

void setup()
{
    startCounter(t1, fastCount, 1'000);   // every 1 ms
    startCounter(t2, slowCount, 100'000); // every 100 ms
}
```

!!! warning "Capture by value"
    Capture by value unless the callback has to change the variable. A capture by reference of a local
    variable, like the loop counter `i` above or a variable of `setup()`, refers to memory that is
    reused for other purposes as soon as the function returns. The callback then reads arbitrary values.

## Functors

A *functor* is an object of a class with a function call operator `operator()`. Like a lambda
expression, it stores data and is invoked like a function; in fact, the compiler turns every lambda
expression into a functor. Since lambda expressions exist, functors are rarely written by hand. They
are still useful if a callback with settings is needed in several places, or if a suitable class exists
already:

```cpp
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
// ...
t1.begin(PulseGenerator(1, 5));  //  5 µs pulse on pin 1
t2.begin(PulseGenerator(2, 10)); // 10 µs pulse on pin 2
```

## Member functions

A member function can't be used as callback directly: it needs an object to be called on, and
`begin()` would not know which one. The callback has to carry this information. This is done with a
lambda expression, which calls the member function on the intended object:

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
    t1.begin([] { b1.blink(); }, 1ms);   // the lambda is the callback, it calls blink() on b1
    t2.begin([] { b2.blink(); }, 250ms); // ... and on b2
}

void loop()
{
}
```

## Timers inside a class

In the example above, the timers are only needed to call `blink()`. It is cleaner to make the timer a
member of the class. Inside a member function, the pointer `this` refers to the object the function was
called on. A lambda expression can capture `this`, which gives the callback access to the member
variables and member functions of this object:

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
        timer.begin([this] { this->blink(); }, periodMs * 1000);
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

`this->` may be omitted: `[this] { blink(); }` does the same.

The class now hides the timer completely. The sketch doesn't need to know anything about timers or
lambdas; it just creates and starts `Blinker` objects. Hiding such implementation details behind a
small interface (*encapsulation*) is one of the main principles of well written C++ programs. It pays
off as a program grows: the parts of a large program can be developed, tested and changed
independently. The `Blinker` could switch to another timer, or to no timer at all, without a change to
the sketches that use it.

## Callbacks and the rest of the sketch

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

---

## Advanced

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
