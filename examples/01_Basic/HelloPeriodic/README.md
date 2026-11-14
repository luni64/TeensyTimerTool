# HelloPeriodic

The smallest TeensyTimerTool sketch: a periodic timer toggles the built-in LED every 250 ms. The timer
runs in the background; `loop()` stays empty and free for the application.

**What you learn**

- How to declare a `PeriodicTimer` without choosing a specific hardware timer: it takes the next free
  timer from the timer pool.
- How to start it with `begin(callback, period)`; the period is given in microseconds.

**Boards:** all Teensy boards. **Hardware:** none, the sketch uses the built-in LED.

**Result:** the LED blinks at 2 Hz.

**Manual:** [Getting started](https://luni64.github.io/TeensyTimerTool/#a-first-periodic-timer),
[Usage – Periodic timers](https://luni64.github.io/TeensyTimerTool/usage/#periodic-timers)
