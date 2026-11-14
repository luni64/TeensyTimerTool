# Timer types

A timer type is passed to the constructor of `PeriodicTimer` or `OneShotTimer` to select a specific
timer: `PeriodicTimer t1(PIT);`. Each name stands for a timer module; `begin()` takes the next free
channel of this module. The properties of the timers are compared in [Choosing a timer](../choosing-a-timer.md).

## Available timer types

| Name | Boards | Channels | Description |
|---|---|---|---|
| `GPT1`, `GPT2` | Teensy 4 | 1 each | General Purpose Timer, 32 bit |
| `TMR1` … `TMR4` | Teensy 4 | 4 each | Quad Timer, 16 bit |
| `PIT` | Teensy 4 | 4 | Periodic Interrupt Timer, 32 bit |
| `RTC` | Teensy 4 | 1 | periodic interrupts of the real time clock |
| `FTM0`, `FTM1` | Teensy 3 | 8, 2 | Flex Timer Module, 16 bit |
| `FTM2` | Teensy 3.1, 3.2, 3.5, 3.6 | 2 | Flex Timer Module, 16 bit |
| `FTM3` | Teensy 3.5, 3.6 | 8 | Flex Timer Module, 16 bit |
| `TCK` (also `TCK32`) | all | 20 (shared) | software timer, 32 bit cycle counter (Teensy LC: `micros()`) |
| `TCK64` | Teensy 3, 4 | (shared) | software timer, 64 bit cycle counter |
| `TCK_RTC` | Teensy 4 | (shared) | software timer, real time clock |

`FTM4` is an alias of `FTM3` on Teensy 3.5 and 3.6. All TCK types share 20 channels; the number can be
changed in the [configuration](../configuration.md#number-of-software-timers).

## Supported functions

| Function | GPT | TMR | PIT | RTC | TCK, TCK64 | TCK_RTC | FTM |
|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| `PeriodicTimer` | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| `OneShotTimer` | ✓ | ✓ | ✓ | – | ✓ | ✓ | ✓ |
| `setPeriod()` | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | – |
| `setPeriod(…, keepElapsed)` | – | ✓ | ✓ | – | ✓ | ✓ | – |
| `setNextPeriod()` | – | ✓ | ✓ | – | ✓ | ✓ | – |
| `resume()` | ✓ | ✓ | ✓ | – | ✓ | ✓ | – |
| `setPrescaler()` | ✓ | ✓ | – | – | – | – | – |
| `triggerDirect()`, `getTriggerReload()` | ✓ | ✓ | ✓ | – | ✓ | ✓ | ✓ |
| `getPeriod()`, `getResolution()` | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ | – |
| `getRemainingTime()` | ✓ | ✓ | ✓ | – | ✓ | ✓ | – |

Unsupported functions return `errorCode::notSupported` (getters: `NAN`) and don't change the timer.
`begin()`, `start()`, `stop()`, `end()` and `getMaxPeriod()` are supported by all timers.

## Timer pool

A timer without timer type takes the next free timer from the pool. Default pools:

| Board | Pool, in this order |
|---|---|
| Teensy 4.0, 4.1, MicroMod | GPT1, GPT2, TMR1, TMR2, TMR3, TMR4, TCK |
| Teensy 3.6 | FTM0, FTM1, FTM2, FTM3, TCK |
| Teensy 3.5 | FTM0, FTM1, FTM2, FTM3, TCK |
| Teensy 3.1, 3.2 | FTM0, FTM1, FTM2, TCK |
| Teensy 3.0 | FTM0, FTM1, TCK |
| Teensy LC | TCK |

The pool can be changed in the [configuration](../configuration.md#timer-pool).

<!-- tests: api capability table (TeensyTimerTool-TestSuite suites/api/api.cpp), api_pool_order, api_pool_exhausted -->
