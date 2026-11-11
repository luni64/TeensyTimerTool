# Getting started

!!! concept inline end "Timer pool"
    The library keeps a list of the timers it may use freely, the *timer pool*. A timer declared without
    naming a specific hardware timer takes the next free one from this list. The content of the pool
    can be changed in the [configuration](configuration.md).

TeensyTimerTool provides a common interface to most timers of the Teensy boards: the hardware timers
(GPT, TMR, PIT and RTC on Teensy 4, FTM on Teensy 3) and up to 20 software timers (TCK), which are
available on every Teensy. Each timer can be used as periodic or as one-shot timer. A sketch can either
request a specific timer or take the next free timer from a pool.

In addition to the `IntervalTimer` of the Teensy core, TeensyTimerTool offers

- a large number of timers (more than 40 on Teensy 4),
- one-shot timers, which invoke a function once after a delay,
- periods and delays as numbers (microseconds) or with units: `250ms`, `20us`, `1.5s`,
- callbacks with state: lambdas, member functions, functors.

The overhead is low: invoking a callback through TeensyTimerTool takes about as long as programming the
timer registers directly ([comparison](advanced/index.md#efficiency)).

!!! note "Work in progress"
    The documentation is being moved here from the [wiki](https://github.com/luni64/TeensyTimerTool/wiki)
    and updated for v1.5.

## Installation

=== "Arduino IDE"

    Open the Library Manager (books icon in the left side bar, or *Tools → Manage Libraries…*), search
    for **TeensyTimerTool** and click *Install*.

=== "PlatformIO"

    Add the library to `platformio.ini`:

    ```ini
    lib_deps = luni64/TeensyTimerTool
    ```

=== "Manual"

    Download the [latest release](https://github.com/luni64/TeensyTimerTool/releases/latest) as zip
    file and install it with *Sketch → Include Library → Add .ZIP Library…*.

## A first periodic timer

The following sketch takes a timer from the pool and toggles the built-in LED every 250 ms:

```cpp
--8<-- "01_Basic/HelloPeriodic/HelloPeriodic.ino"
```

The sketch declares a `PeriodicTimer` and calls `begin()` in `setup()` with the function to be invoked
(the *callback*) and the period in microseconds. The timer then runs in the background; `loop()` remains
available for the application.

!!! tip
    The period can also be written as `250ms`. Units, starting and stopping, changing the period and
    further functions are described in [Basic usage](basic-usage.md).

## A first one-shot timer

A `OneShotTimer` invokes its callback once, after a delay. The delay is passed to `trigger()`, which
starts the timer and returns immediately. Unlike `delay()`, the sketch is not paused: the program
continues, and the callback is invoked in the background when the delay has expired.

In the following sketch, `loop()` switches the LED on every 500 ms and triggers the timer, which switches
the LED off again after 10 ms:

```cpp
--8<-- "01_Basic/HelloOneShot/HelloOneShot.ino"
```

## Selecting a timer

`PeriodicTimer t1;` takes the next free timer from the timer pool. On Teensy 4, the pool contains the
hardware timers GPT1, GPT2 and TMR1 … TMR4, followed by the software timers (TCK); on Teensy 3, it
starts with the FTM timers. This is sufficient for most applications. The pool can be changed in the
[configuration](configuration.md).

A specific timer, e.g. for very long periods or a particularly fine resolution, is selected by passing
it to the constructor:

```cpp
PeriodicTimer t1(TCK);  // software timer, available on every Teensy
PeriodicTimer t2(PIT);  // PIT channel (Teensy 4)
OneShotTimer  t3(GPT1); // GPT1 timer (Teensy 4)
```

[Choosing a timer](choosing-a-timer.md) compares the properties of all timers.

## Further reading

- [Basic usage](basic-usage.md): starting, stopping and changing timers.
- [Callbacks](callbacks.md): lambdas, member functions, timers in classes.
- [Error handling](error-handling.md): evaluating the codes returned by `begin()` and other functions.
- [Examples](examples.md): the example sketches of the library.

Questions, bug reports and feature requests are welcome in the [PJRC forum](https://forum.pjrc.com) or
in the [issue tracker](https://github.com/luni64/TeensyTimerTool/issues).
