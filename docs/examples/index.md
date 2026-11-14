# Examples

The library comes with the following example sketches. In the Arduino IDE they are available under
*File → Examples → TeensyTimerTool*. The names in the tables link to the sources in the
[examples folder](https://github.com/luni64/TeensyTimerTool/tree/master/examples) of the repository.

## Basic

| Example | Shows | Boards |
|---|---|---|
| [`HelloPeriodic`][HelloPeriodic] | a periodic timer from the pool blinks the LED | all |
| [`HelloOneShot`][HelloOneShot] | a one-shot timer switches the LED off after 3 s while `loop()` keeps running | all |
| [`MoreTimers`][MoreTimers] | TCK, TMR and GPT timers side by side, a periodic timer triggering a one-shot timer | Teensy 4 |
| [`ArrayOfTimersSimple`][ArrayOfTimersSimple] | an array of three software timers printing to the serial monitor | all |
| [`ArrayOfTimers`][ArrayOfTimers] | an array of one-shot timers of different types generating pulses of different lengths | Teensy 4 |
| [`UsingChronoDurations1`][UsingChronoDurations1], [`UsingChronoDurations2`][UsingChronoDurations2] | periods and delays as `std::chrono` durations (`25ms`, `0.5s + 10ms` …) | all |
| [`RTC`][RTC] | the RTC timer: periods of 2⁻ⁿ s in sync with the real time clock, `stop()` and `start()` | Teensy 4 |
| [`RTC_TCK_Timer`][RTC_TCK_Timer] | a TCK_RTC timer measures the drift of the main crystal against the RTC crystal | Teensy 4 |

## Advanced

| Example | Shows | Boards |
|---|---|---|
| [`UsingLambdas`][UsingLambdas] | a lambda expression as callback | all |
| [`CapturingLambdas`][CapturingLambdas] | lambdas capturing a loop variable: ten timers with one callback function | all |
| [`CallbackWithParams`][CallbackWithParams] | calling a function with parameters from a callback | all |
| [`TimersInContainers`][TimersInContainers] | timers in a `std::vector`, added and removed at runtime over the serial monitor | all |
| [`Prescaler`][Prescaler] | `setPrescaler()` on TMR and GPT timers: resolution, maximum period, changing it while running | Teensy 4 |

## Applications

| Example | Shows | Boards |
|---|---|---|
| [`DoubleExposure`][DoubleExposure] | controlling two pulsed lasers and a camera with one-shot timers | Teensy 3 |

## Miscellaneous

| Example | Shows | Boards |
|---|---|---|
| [`PinInformation`][PinInformation] | prints which timer generates the PWM signal of each pin, see [Avoiding PWM timer clashes](../advanced/pwm-timer-clashes.md) | all |

[HelloPeriodic]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/01_Basic/HelloPeriodic
[HelloOneShot]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/01_Basic/HelloOneShot
[MoreTimers]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/01_Basic/MoreTimers
[ArrayOfTimersSimple]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/01_Basic/ArrayOfTimersSimple
[ArrayOfTimers]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/01_Basic/ArrayOfTimers
[UsingChronoDurations1]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/01_Basic/UsingChronoDurations1
[UsingChronoDurations2]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/01_Basic/UsingChronoDurations2
[RTC]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/01_Basic/RTC
[RTC_TCK_Timer]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/01_Basic/RTC_TCK_Timer
[UsingLambdas]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/02_Advanced/UsingLambdas
[CapturingLambdas]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/02_Advanced/CapturingLambdas
[CallbackWithParams]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/02_Advanced/CallbackWithParams
[TimersInContainers]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/02_Advanced/TimersInContainers
[Prescaler]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/02_Advanced/Prescaler
[DoubleExposure]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/03_Applications/DoubleExposure
[PinInformation]: https://github.com/luni64/TeensyTimerTool/tree/master/examples/99_Misc/PinInformation
