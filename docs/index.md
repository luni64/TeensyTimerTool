# Getting started

TeensyTimerTool gives all timers of your Teensy the same, easy to use interface: the hardware timers
(GPT, TMR, PIT, FTM, RTC) and up to 20 software timers (TCK). Every timer can run periodically or as a
one-shot timer, and you can either pick a specific timer or just take the next free one from a pool.

!!! note "Work in progress"
    The documentation is moving here from the [wiki](https://github.com/luni64/TeensyTimerTool/wiki)
    and is being updated for v1.5.

## Your first timer

This sketch takes a timer from the pool and toggles the built-in LED every 250 ms:

```cpp
--8<-- "01_Basic/HelloPeriodic/HelloPeriodic.ino"
```
