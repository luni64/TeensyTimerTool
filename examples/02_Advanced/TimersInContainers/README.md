# TimersInContainers

A set of blinking pins, controlled from the serial monitor. Each blinker owns a `PeriodicTimer` and is
stored in a `std::vector`; blinkers are added and removed while the sketch runs.

| Command | Effect |
|---|---|
| `a <pin> <ms>` | add a blinker, e.g. `a 13 250` toggles the LED every 250 ms |
| `r <pin>` | remove the blinker of this pin |
| `l` | list the blinkers |

**What you learn**

- That timers can be stored in standard containers. Timers can't be copied, but they can be moved: when
  the vector grows or an element is erased, the timers are moved and keep running.
- That a removed timer stops and releases its channel, which is then free for other timers.
- How the return codes report an empty timer pool (`noFreeModule`) or a period that is too long for the
  timer (`periodOverflow`).

**Boards:** all Teensy boards. On Teensy 3.x, the pool starts with the FTM timers, whose maximum period is
about 35 ms; longer periods are clipped to this value.

**Hardware:** none. Every second, the sketch prints how often each pin toggled; an LED or an oscilloscope
on the pins shows the same.

**Manual:** [Usage – Arrays and containers of timers](https://luni64.github.io/TeensyTimerTool/usage/#arrays-and-containers-of-timers)
