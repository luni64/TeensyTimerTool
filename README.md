[![PlatformIO Registry](https://badges.registry.platformio.org/packages/luni64/library/TeensyTimerTool.svg)](https://registry.platformio.org/libraries/luni64/TeensyTimerTool)
[![arduino-library-badge](https://www.ardu-badge.com/badge/TeensyTimerTool.svg?)](https://www.ardu-badge.com/TeensyTimerTool)

# TeensyTimerTool

TeensyTimerTool provides a common, easy to use interface to the hardware timers of the PJRC Teensy boards
(GPT, TMR, PIT and RTC on Teensy 4, FTM on Teensy 3) and to up to 20 software timers, which are available on
every Teensy. Every timer can be used as periodic or as one-shot timer, with periods given in microseconds
or with units (`250ms`, `1.5s`). Callbacks can be plain functions, lambda expressions or functors.

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

PeriodicTimer t1; // takes the next free timer

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    t1.begin([] { digitalToggleFast(LED_BUILTIN); }, 250ms);
}

void loop()
{
}
```

## Documentation

- **[Manual](https://luni64.github.io/TeensyTimerTool/)**: getting started, usage, choosing a timer,
  callbacks, configuration, complete API reference
- **[Upgrading from 1.4](https://luni64.github.io/TeensyTimerTool/upgrading/)**: changes in version 1.5
- **[Change history](https://github.com/luni64/TeensyTimerTool/releases)**: release notes of all versions
- **[Examples](examples)**: every example folder contains a README explaining what the example shows
- **[Forum thread](https://forum.pjrc.com/threads/59112-TeensyTimerTool)** on the PJRC forum

## Supported boards

Teensy 4.0, 4.1, MicroMod, Teensy 3.0 … 3.6 and Teensy LC (software timers only). Teensy 3.x and LC are in
maintenance mode: bugs are fixed, new features are added for Teensy 4 only.

Requires Teensyduino 1.59 or later. For Teensyduino 1.58 and earlier, use TeensyTimerTool 1.3.0.

## Installation

- **Arduino IDE:** Library Manager, search for *TeensyTimerTool*
- **PlatformIO:** `lib_deps = luni64/TeensyTimerTool`

## License

MIT, see [LICENSE](LICENSE).
