# UsingChronoDurations1

Two software timers with periods given as `std::chrono` durations: one prints a message every 5.02 s, the
other toggles the built-in LED every 25 ms.

**What you learn**

- How to write periods with units instead of microseconds: `5.02s` instead of `5'020'000`, `25ms` instead
  of `25'000`.
- That the conversion is done by the compiler: durations cost nothing at runtime.

**Boards:** all Teensy boards. **Hardware:** none, the sketch uses the built-in LED and the serial monitor.

**Output:** the LED flickers at 20 Hz; every 5.02 s, the serial monitor shows `called @: <time> ms`.

**Manual:** [Usage – Specifying periods](https://luni64.github.io/TeensyTimerTool/usage/#specifying-periods)
