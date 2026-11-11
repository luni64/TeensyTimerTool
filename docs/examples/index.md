# Examples

The library comes with the following example sketches. In the Arduino IDE they are available under
*File → Examples → TeensyTimerTool*; the sources are in the
[examples folder](https://github.com/luni64/TeensyTimerTool/tree/master/examples) of the repository.

## Basic

| Example | Shows | Boards |
|---|---|---|
| `HelloPeriodic` | a periodic timer from the pool blinks the LED | all |
| `HelloOneShot` | a one-shot timer switches the LED off after 3 s while `loop()` keeps running | all |
| `MoreTimers` | TCK, TMR and GPT timers side by side, a periodic timer triggering a one-shot timer | Teensy 4 |
| `ArrayOfTimers` | an array of one-shot timers generating pulses of different lengths | all |
| `UsingChronoDurations1`, `UsingChronoDurations2` | periods and delays as `std::chrono` durations (`25ms`, `0.5s + 10ms` …) | all |
| `RTC` | the RTC timer: periods of 2⁻ⁿ s in sync with the real time clock, `stop()` and `start()` | Teensy 4 |
| `RTC_TCK_Timer` | a TCK_RTC timer measures the drift of the main crystal against the RTC crystal | Teensy 4 |

## Advanced

| Example | Shows | Boards |
|---|---|---|
| `UsingLambdas` | a lambda expression as callback | all |
| `CapturingLambdas` | lambdas capturing a loop variable: ten timers with one callback function | all |
| `CallbackWithParams` | calling a function with parameters from a callback | all |
| `TimersInContainers` | timers in a `std::vector`, added and removed at runtime over the serial monitor | all |
| `Prescaler` | `setPrescaler()` on TMR and GPT timers: resolution, maximum period, changing it while running | Teensy 4 |

## Applications

| Example | Shows | Boards |
|---|---|---|
| [`DoubleExposure`](double-exposure.md) | controlling two pulsed lasers and a camera with one-shot timers, step by step | Teensy 3 |

## Miscellaneous

| Example | Shows | Boards |
|---|---|---|
| `PinInformation` | prints which timer generates the PWM signal of each pin, see [Avoiding PWM timer clashes](../advanced/pwm-timer-clashes.md) | all |
