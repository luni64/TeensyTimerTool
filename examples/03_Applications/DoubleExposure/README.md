# DoubleExposure

Controls two pulsed lasers and a camera for double-exposure photography, e.g. particle image velocimetry.
Each laser needs a pre-trigger pulse and, after a warm-up time, a trigger pulse; the camera needs an
exposure pulse. The second laser fires 100 … 500 µs after the first (the *exposure delay*). The sequence
repeats at 15 Hz.

The sketch is split into small classes:

| Class | File | Job |
|---|---|---|
| `PulseGenerator` | PulseGenerator.h | generates one pulse of given width after a given delay, with a one-shot timer that re-triggers itself |
| `LaserController` | LaserController.h | schedules the pre-trigger, trigger and camera pulses of one laser |
| `SystemController` | SystemController.h | starts both lasers with the exposure delay, single shots or continuously at 15 Hz with a periodic timer |

**What you learn**

- How to build an application from classes that each own their timers, with a lambda capturing `this`
  as callback.
- How to generate pulse sequences with one-shot timers that re-trigger themselves from the callback.
- How to configure a timer with `begin(…, false)` and start and stop it later.

**Boards:** Teensy 3.x. The six pulse generators use six of the eight channels of the FTM0 module.

**Hardware:** an oscilloscope or logic analyzer on pins 1 … 5 shows the sequence. The commented code in
DoubleExposure.ino reads the exposure delay from a potentiometer on A0 (needs the ResponsiveAnalogRead
library).

**Manual:** [Callbacks – Timers inside a class](https://luni64.github.io/TeensyTimerTool/callbacks/#timers-inside-a-class)
