# UsingChronoDurations2

Four one-shot timers are triggered with delays written as `std::chrono` expressions, and each prints the
time at which it fired. A periodic timer blinks the LED meanwhile.

**What you learn**

- That durations can be calculated with: `0.5s + 10ms`, `2.5 * 0.3s + 20'000us / 2`,
  `milliseconds(50) + microseconds(5000)`.
- How to attach the same callback to all timers of an array with a range-based `for` loop.

**Boards:** all Teensy boards. **Hardware:** none, the sketch uses the built-in LED and the serial monitor.

**Output:** the four timers fire after 10, 55, 510 and 760 ms:

```
called @: 10 ms
called @: 55 ms
called @: 510 ms
called @: 760 ms
```

**Manual:** [Usage – Specifying periods](https://luni64.github.io/TeensyTimerTool/usage/#specifying-periods)
