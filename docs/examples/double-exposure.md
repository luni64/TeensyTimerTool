# Double exposure laser illuminator

![Timing of the lasers and the camera](https://raw.githubusercontent.com/wiki/luni64/TeensyTimerTool/ExamplesFiles/DoubleExposure/overview.png)

This application example controls two pulsed lasers and a camera to capture two laser illuminated images
in quick succession. As shown in the timing diagram above, each laser needs a pre-trigger before its main
trigger fires the laser pulse. The camera shutter has to be open while the laser illuminates the scene,
but only as long as necessary to keep the image noise low.

The example is written for Teensy 3 (it uses FTM timers); the structure applies to any board. The
complete code is in the folder
[examples/03_Applications/DoubleExposure](https://github.com/luni64/TeensyTimerTool/tree/master/examples/03_Applications/DoubleExposure).

## Structure

The task is split into three small classes, each of which can be tested on its own:

- `PulseGenerator` generates a single pulse with a given delay and width on one pin.
- `LaserController` controls one laser and the camera: it schedules the pre-trigger, the main trigger and
  the camera pulse, using three `PulseGenerator` objects.
- `SystemController` synchronizes the two `LaserController` objects to obtain the required time between
  the two exposures, and provides continuous shooting with an adjustable exposure delay.

![Classes](https://raw.githubusercontent.com/wiki/luni64/TeensyTimerTool/ExamplesFiles/DoubleExposure/classes.png)

## The PulseGenerator class

```cpp
--8<-- "03_Applications/DoubleExposure/PulseGenerator.h"
```

`schedulePulse(delay, width)` sets up the internal one-shot timer and returns immediately. For an
immediate pulse (`delay == 0`), it sets the pin high and triggers the timer with the pulse width; the
callback then sets the pin low. For a delayed pulse, it triggers the timer with the delay; the callback
sets the pin high and re-triggers the timer with the pulse width.

The callback uses the state of the pin to tell the two cases apart: a low pin means the delay has expired
and the pulse starts, a high pin means the pulse ends.

A short test of the class generates three pulses on pins 1 … 3:

```cpp
#include "PulseGenerator.h"

PulseGenerator g1, g2, g3;

void setup()
{
    g1.begin(1);
    g2.begin(2);
    g3.begin(3);
}

void loop()
{
    g1.schedulePulse(0, 2'000);      // immediate 2 ms pulse on pin 1
    g2.schedulePulse(5'000, 10'000); // 10 ms pulse on pin 2, starting 5 ms after the first one
    g3.schedulePulse(1'000, 5'000);  // 5 ms pulse on pin 3, starting 1 ms after the first one

    delay(50);
}
```

![Measured pulses of the PulseGenerator test](https://raw.githubusercontent.com/wiki/luni64/TeensyTimerTool/ExamplesFiles/DoubleExposure/UnitTestPulseGen.jpg)

## The LaserController class

```cpp
--8<-- "03_Applications/DoubleExposure/LaserController.h"
```

`shoot()` fires one laser and opens the camera shutter at the right time: it schedules an immediate
pre-trigger pulse, a delayed main trigger pulse and a delayed camera pulse.

The correction terms in the timing constants compensate the execution time of `schedulePulse()` and of
the underlying `trigger()` calls. They were determined experimentally. Precomputed reload values
(`triggerDirect()`) and a smaller prescaler for FTM0 would reduce them.

Result for 140 µs warm-up time, 130 µs camera delay, 10 µs pulse width and 30 µs shutter time:

![Measured pulses of the LaserController test](https://raw.githubusercontent.com/wiki/luni64/TeensyTimerTool/ExamplesFiles/DoubleExposure/UnitTestLaserCtrl.jpg)

## The SystemController class

```cpp
--8<-- "03_Applications/DoubleExposure/SystemController.h"
```

`shoot()` synchronizes the two lasers. It fires the first laser and waits until the exposure delay has
elapsed before firing the second one. Since `LaserController::shoot()` takes a few microseconds to
execute, a stopwatch started before the first shot gives a more accurate delay than a plain
`delayMicroseconds()` between the shots.

`setExposureDelay()` changes the time between the two exposures, and `continuosMode()` starts or stops a
periodic TCK timer which calls `shoot()` at a rate of 15 Hz.

## Result

Pulses generated with the following settings (see the timing diagram at the top):

| Time | Value |
|---|---:|
| t<sub>warmup</sub> | 140 µs |
| t<sub>cam-delay</sub> | 130 µs |
| t<sub>delta</sub> | 500 µs |
| t<sub>p</sub> | 10 µs |
| t<sub>int</sub> | 30 µs |

![Measured timing of the complete system](https://raw.githubusercontent.com/wiki/luni64/TeensyTimerTool/ExamplesFiles/DoubleExposure/timing_measurement1.jpg)
