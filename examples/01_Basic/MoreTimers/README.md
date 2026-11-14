# MoreTimers

Four timers of different types run side by side:

| Timer | Type | Job |
|---|---|---|
| `t1` | TCK (software timer) | 200 ns pulse on pin 1 every 50 ms |
| `t2` | TMR1, first channel | 400 ns pulse on pin 2 every 100 µs |
| `t3` | GPT1 | switches the LED on every second and triggers `t4` |
| `t4` | TMR1, second channel (one-shot) | switches the LED off 10 ms later |

**What you learn**

- How to request specific timers: a software timer, channels of a TMR module, a GPT timer.
- How a periodic timer can trigger a one-shot timer from its callback.
- That periods are rounded to the resolution of the timer: the 100 µs of the TMR timer become 99.84 µs
  with the default prescaler.

**Boards:** Teensy 4.0, 4.1, MicroMod. **Hardware:** an oscilloscope or logic analyzer on pins 1 and 2;
the LED flashes briefly once per second.

**Manual:** [Choosing a timer](https://luni64.github.io/TeensyTimerTool/choosing-a-timer/),
[Resolution and rounding](https://luni64.github.io/TeensyTimerTool/choosing-a-timer/#resolution-and-rounding)
