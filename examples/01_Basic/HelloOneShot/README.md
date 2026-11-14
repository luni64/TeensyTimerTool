# HelloOneShot

`setup()` switches the built-in LED on and triggers a one-shot timer, which switches it off again after
3 s. `trigger()` returns immediately: while the timer runs, `loop()` keeps printing the time and the LED
state to the serial monitor.

**What you learn**

- How a `OneShotTimer` invokes its callback once, after the delay passed to `trigger()`.
- The difference to `delay()`: the sketch is not paused while the timer runs.
- How to request a specific timer type, here a software timer (`TCK`), which supports long delays on
  every board.

**Boards:** all Teensy boards. **Hardware:** none, the sketch uses the built-in LED and the serial monitor.

**Output** (serial monitor): the LED state switches from `on` to `off` about 3 s after the start.

```
  374 ms: LED on
  624 ms: LED on
  ...
 3124 ms: LED on
 3374 ms: LED off
 3624 ms: LED off
```

**Manual:** [Getting started](https://luni64.github.io/TeensyTimerTool/#a-first-one-shot-timer),
[Usage – One-shot timers](https://luni64.github.io/TeensyTimerTool/usage/#one-shot-timers)
