# RTC_TCK_Timer

A `TCK_RTC` timer, which counts the 32.768 kHz crystal of the real time clock, fires every 0.5 s. The
callback compares the elapsed time with `micros()`, which is derived from the main 24 MHz crystal, and
prints the relative drift of the two crystals.

**What you learn**

- How to use `TCK_RTC`, a software timer with the real time clock as time base.
- That two crystals never run at exactly the same frequency: the drift is typically a few ppm.

**Boards:** Teensy 4.0, 4.1, MicroMod. **Hardware:** none, the output goes to the serial monitor.

**Output** (Teensy 4.1; the values depend on the board):

```
t: 500001 µs,  rel. drift: 2.00 ppm
t: 1000002 µs,  rel. drift: 2.00 ppm
t: 1500004 µs,  rel. drift: 2.67 ppm
t: 2000005 µs,  rel. drift: 2.50 ppm
...
```

**Manual:** [Choosing a timer – RTC and TCK_RTC](https://luni64.github.io/TeensyTimerTool/choosing-a-timer/#rtc-and-tck_rtc)
