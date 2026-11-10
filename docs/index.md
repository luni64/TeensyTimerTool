# Getting started

TeensyTimerTool gives all timers of your Teensy the same, easy to use interface. You get the hardware
timers of the board (GPT, TMR, PIT and RTC on a Teensy 4, FTM on a Teensy 3) plus up to 20 software timers
(TCK) which work on every Teensy. Every timer can call a function periodically or once after a delay, and
you can either pick a specific timer or simply take the next free one.

Compared to the `IntervalTimer` of the Teensy core you get

- many more timers (more than 40 on a Teensy 4),
- one-shot timers (call a function once, e.g. 10 ms after a button was pressed),
- periods and delays as plain numbers or in units: `250ms`, `20us`, `1.5s`,
- callbacks with state: lambdas, member functions, functors.

And it is fast: calling a callback through TeensyTimerTool costs about the same as programming the timer
registers yourself.

!!! note "Work in progress"
    The documentation is moving here from the [wiki](https://github.com/luni64/TeensyTimerTool/wiki)
    and is being updated for v1.5.

## Installation

=== "Arduino IDE"

    Open the Library Manager (*Tools → Manage Libraries…*), search for **TeensyTimerTool** and click
    *Install*.

=== "PlatformIO"

    Add the library to your `platformio.ini`:

    ```ini
    lib_deps = luni64/TeensyTimerTool
    ```

=== "Manual"

    Download the [latest release](https://github.com/luni64/TeensyTimerTool/releases/latest) as zip
    file and add it with *Sketch → Include Library → Add .ZIP Library…*, or unpack it into your
    libraries folder.

## Your first periodic timer

This sketch takes a timer from the pool of free timers and toggles the built-in LED every 250 ms:

```cpp
--8<-- "01_Basic/HelloPeriodic/HelloPeriodic.ino"
```

That's all you need: create a `PeriodicTimer`, and in `setup()` call `begin()` with the function to call
(the *callback*) and the period in microseconds. The timer runs in the background, `loop()` stays free for
your own code.

!!! tip
    Instead of `250'000` you can also write `250ms`. Units, starting and stopping, changing the period
    and more: see [Basic usage](basic-usage.md).

## Your first one-shot timer

A `OneShotTimer` calls its callback only once, after the delay you pass to `trigger()`. Here `loop()`
switches the LED on every 500 ms and triggers the timer, which switches it off again 10 ms later. The
LED flashes briefly:

```cpp
--8<-- "01_Basic/HelloOneShot/HelloOneShot.ino"
```

## Which timer do I get?

`PeriodicTimer t1;` takes the next free timer from the *timer pool*. On a Teensy 4 the hardware timers
GPT1, GPT2 and TMR1 … TMR4 come first, the software timers (TCK) after them; on a Teensy 3 the FTM timers
come first. For most sketches this is just fine.

!!! warning "Teensy 3.x: long periods"
    The FTM timers of a Teensy 3 reach only about 35 … 45 ms. With a longer period, `begin()` returns the
    warning `periodOverflow` and the timer runs with its maximum period, so the LED of the first example
    blinks much faster than intended. Use a software timer for long periods: `PeriodicTimer t1(TCK);`

If you need a particular timer, e.g. for a very long period or a very fine resolution, name it:

```cpp
PeriodicTimer t1(TCK);  // a software timer, works on every Teensy
PeriodicTimer t2(PIT);  // a PIT channel (Teensy 4)
OneShotTimer  t3(GPT1); // the GPT1 timer (Teensy 4)
```

[Choosing a timer](choosing-a-timer.md) compares the timers and helps you pick one.

## Where to go next

- [Basic usage](basic-usage.md): everything about starting, stopping and changing timers.
- [Callbacks](callbacks.md): lambdas, member functions, timers inside your own classes.
- [Error handling](error-handling.md): what to do when `begin()` doesn't return `OK`.
- [Examples](examples.md): all example sketches of the library.

Questions? The [PJRC forum](https://forum.pjrc.com) is the best place to ask, bugs and ideas go to the
[issue tracker](https://github.com/luni64/TeensyTimerTool/issues).
