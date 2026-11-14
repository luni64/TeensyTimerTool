# RTC

The RTC timer uses the real time clock to blink the built-in LED. The sketch runs through several
periods, stops the timer for 2 s and changes the period while the timer runs.

**What you learn**

- That the RTC timer supports periods of 2⁻ⁿ s only (1 s, 0.5 s, 0.25 s … 30.5 µs) and selects the next
  longer one: 25 ms becomes 31.25 ms, 50 ms becomes 62.5 ms.
- That its callbacks are synchronized with the real time clock: a 1 s period fires exactly at the second
  transitions of the clock.
- How to use `stop()` and `setPeriod()`.

**Boards:** Teensy 4.0, 4.1, MicroMod. **Hardware:** none, the sketch uses the built-in LED.

**Result:** the LED blinks at 16 Hz for 1.5 s, pauses for 2 s, blinks at 1 Hz for 5 s, then at 8 Hz.

**Manual:** [Choosing a timer – RTC and TCK_RTC](https://luni64.github.io/TeensyTimerTool/choosing-a-timer/#rtc-and-tck_rtc)
