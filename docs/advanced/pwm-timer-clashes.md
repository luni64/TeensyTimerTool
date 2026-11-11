# Avoiding PWM timer clashes

`analogWrite()` generates its PWM signals with hardware timers. Some of these timers are also used by
TeensyTimerTool: the TMR (QuadTimer) modules on Teensy 4 and the FTM modules on Teensy 3. A module can
either generate PWM or serve as timer for TeensyTimerTool, not both.

**Rule:** don't use `analogWrite()` on the pins of a TMR or FTM module that TeensyTimerTool uses.

When TeensyTimerTool takes the first timer of such a module, it reconfigures the whole module: on
Teensy 4 it stops all four channels of the TMR module, on Teensy 3 it sets the counter and prescaler of
the FTM module, which are shared by all its channels. PWM on the pins of this module stops or gets a
wrong frequency.

!!! warning "Timers from the pool"
    The pool uses these modules too. On Teensy 4, the third and all following pool timers are TMR
    channels (TMR1 first: pins 10, 11, 12); on Teensy 3, the pool starts with FTM0 (pins 5, 6, 9, 10,
    20 … 23). If a sketch needs PWM on these pins, request TCK or GPT timers explicitly, or remove the
    modules from the pool in the [configuration](../configuration.md#timer-pool).

## Pins per timer module

Only the modules TeensyTimerTool can use are listed. The other PWM pins (FlexPWM on Teensy 4, TPM on
Teensy 3.6) can be used freely.

=== "Teensy 4.1"

    | Module | PWM pins |
    |---|---|
    | TMR1 | 10, 11, 12 |
    | TMR2 | 13 |
    | TMR3 | 14, 15, 18, 19 |
    | TMR4 | – |

=== "Teensy 4.0"

    | Module | PWM pins |
    |---|---|
    | TMR1 | 10, 11, 12 |
    | TMR2 | 13 |
    | TMR3 | 14, 15, 18, 19 |
    | TMR4 | – |

=== "Teensy MicroMod"

    | Module | PWM pins |
    |---|---|
    | TMR1 | 10, 11, 12 |
    | TMR2 | 13, 40, 41 |
    | TMR3 | 14, 15, 18, 19 |
    | TMR4 | 45 |

=== "Teensy 3.5, 3.6"

    | Module | PWM pins |
    |---|---|
    | FTM0 | 5, 6, 9, 10, 20, 21, 22, 23 |
    | FTM1 | 3, 4 |
    | FTM2 | 29, 30 |
    | FTM3 | 2, 7, 8, 14, 35, 36, 37, 38 |

=== "Teensy 3.1, 3.2"

    | Module | PWM pins |
    |---|---|
    | FTM0 | 5, 6, 9, 10, 20, 21, 22, 23 |
    | FTM1 | 3, 4 |
    | FTM2 | 25, 32 |

=== "Teensy 3.0"

    | Module | PWM pins |
    |---|---|
    | FTM0 | 5, 6, 9, 10, 20, 21, 22, 23 |
    | FTM1 | 3, 4 |

The GPT, PIT and RTC timers of Teensy 4 and the TCK timers don't generate PWM; they can always be used
together with `analogWrite()`. The Teensy LC generates PWM with TPM timers, which TeensyTimerTool
doesn't use.

Sources: PJRC, [PWM frequency table](https://www.pjrc.com/teensy/td_pulse.html), and the example
`PinInformation` (MicroMod).

## Printing the timer of each pin

The example `PinInformation` prints, for every pin of the board, the timer that generates its PWM signal
and its GPIO register, sorted by pin number, by PWM timer and by GPIO register:

```cpp
--8<-- "99_Misc/PinInformation/PinInformation.ino"
```

Excerpt of the output for a Teensy 4.1, sorted by PWM timer:

```
Pin |  GPIO Reg  |  PWM timer
----|------------|-------------
10  |  GPIO7_00  |  QUAD1
11  |  GPIO7_02  |  QUAD1
12  |  GPIO7_01  |  QUAD1
13  |  GPIO7_03  |  QUAD2
19  |  GPIO6_16  |  QUAD3
18  |  GPIO6_17  |  QUAD3
14  |  GPIO6_18  |  QUAD3
15  |  GPIO6_19  |  QUAD3
...
```

`QUAD1` … `QUAD4` are the modules TeensyTimerTool calls `TMR1` … `TMR4`.
