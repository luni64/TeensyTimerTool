# Prescaler

Two TMR timers and a GPT timer run with different prescalers:

| Output | Timer | Prescaler | Result |
|---|---|---|---|
| pin 0 | TMR1 | `PSC_1` | toggles exactly every 10 µs |
| pin 1 | TMR1 | `PSC_128` (default) | toggles every 10.24 µs (12 ticks of 0.853 µs) |
| LED | GPT1 | `PSC_4096` | blinks at 1 Hz; this timer could run periods of up to 8.5 days |

Every 5 s, the sketch switches the prescaler of the pin 0 timer between `PSC_1` and `PSC_16` while it runs.

**What you learn**

- What the prescaler does: a small value gives a fine resolution but a short maximum period, a large
  value long periods but a coarse resolution.
- How to set it with `setPrescaler()`, before `begin()` or on a running timer, and why the period has to
  be set again after a change on a running timer.
- That invalid values are rejected with `errorCode::argument` (e.g. `PSC_256` on a TMR timer).

**Boards:** Teensy 4.0, 4.1, MicroMod. **Hardware:** an oscilloscope or logic analyzer on pins 0 and 1;
the maximum periods are printed to the serial monitor.

**Manual:** [Choosing a timer – Prescaler](https://luni64.github.io/TeensyTimerTool/choosing-a-timer/#prescaler)
