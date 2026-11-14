# Changelog

All notable changes to TeensyTimerTool are documented on this page. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/); the versions correspond to the tagged
[GitHub releases](https://github.com/luni64/TeensyTimerTool/releases). How to adapt sketches written for
1.4 is described in [Upgrading from 1.4](upgrading.md).

## v1.5.0 - unreleased

**Changes that may need adaptations**

- `setPeriod(p)` restarts the period on all timers. Up to 1.4, TMR and TCK timers kept the elapsed time;
  `setPeriod(p, PeriodMode::keepElapsed)` restores this behaviour.
- Timer objects can't be copied any more (a copy of a started timer crashed the sketch); they can be moved.
- Periods and delays are rounded to the nearest timer tick instead of truncated (PIT, GPT, TMR, TCK). RTC
  periods are rounded up to the next 2⁻ⁿ s as documented.
- `begin(callback, period, false)` no longer starts TMR timers.
- Defined return codes for previously undefined cases: negative periods and delays return
  `invalidArgument`, period 0 returns `zeroPeriod` (the timer is unchanged); `OneShotTimer::start()`
  returns `notSupported` instead of invoking the callback; `begin()` after `setPrescaler()` on a timer
  without prescaler returns the warning `prescalerIgnored`.
- Error codes renamed: `argument` → `invalidArgument`, `callback` → `noCallback`, `reload` → `zeroPeriod`,
  `noFreeModule` → `poolExhausted`, `notImplemented` → `notSupported`. The old names still compile, with a
  deprecation warning.
- `TMR_DEFAULT_PSC` outside `PSC_1` … `PSC_128` is a compile error (was silently truncated).

**New**

- `setPrescaler()` for TMR (`PSC_1` … `PSC_128`) and GPT timers (`PSC_1` … `PSC_4096`), before `begin()` or
  at runtime; new constants `PSC_256` … `PSC_4096` (GPT).
- `PeriodicTimer::resume()`: continues the period interrupted by `stop()`; `start()` restarts it.
- `setPeriod(p, PeriodMode::keepElapsed)` for PIT, TMR and TCK timers.
- `getPeriod()` and `getResolution()`: the period and the tick actually used. `getRemainingTime()` for PIT,
  GPT and TMR timers (was TCK only).
- All getters optionally return `std::chrono` durations, e.g. `t1.getPeriod<std::chrono::microseconds>()`.
- `end()`: stops a timer and releases its channel.
- Timers can be moved and kept in containers such as `std::vector`.
- `triggerDirect()` and `getTriggerReload()` for PIT and TMR timers.
- Code hints: all public functions, error codes, timer types and prescaler constants are documented for
  IntelliSense (VS Code, Visual Studio) and the hover hints of Arduino IDE 2.
- New manual at [luni64.github.io/TeensyTimerTool](https://luni64.github.io/TeensyTimerTool/), replacing the
  wiki. Every example has a README.
- New examples: `ArrayOfTimersSimple`, `HelloOneShot` (rewritten), `Prescaler`, `TimersInContainers`.
  `ArrayOfTimers` now mixes timer types (Teensy 4).

**Changed**

- Too long periods return the warning `periodOverflow` on all timers (was TMR only); the timer runs with its
  maximum period.
- `begin()` on a running timer restarts it with the new callback and period.
- Changing the period of a stopped timer (`setPeriod()`, `setNextPeriod()`) keeps it stopped on all timers;
  the first period after `start()` is the new one.
- The error function attached with `attachErrFunc()` is called exactly once per failed call.
- Lower interrupt overhead and faster `trigger()` on PIT, TMR and GPT, close to hand-written register code
  (see [Performance](advanced/performance.md)).
- TMR: `stop()` also stops the counter of the channel.
- Removed the configuration option `ENABLE_ADVANCED_FEATURES`, which had no effect.

**Fixed**

- Destroying a started PIT or RTC timer object crashed the sketch.
- Starting the RTC timer stopped the real time clock, and with it all TCK_RTC timers.
- `setPeriod()` on a stopped PIT or RTC timer started it.
- A stopped TMR channel invoked its callback when another channel of the same module fired.
- PIT periods longer than about 179 µs were limited to 179 µs (1.4.2, 1.4.3).
- GPT: too long periods were limited to 179 µs; periods shorter than one tick gave the maximum period.
- TMR periods were one tick too long, and wrong at CPU clocks other than 600 MHz.
- `setPrescaler()` caused a linker error.
- `setPeriod(0)` on a running timer hung the board.
- `setPeriod()` / `setNextPeriod()` before `begin()` crashed; they return `notInitialized`.
- A second `PeriodicTimer(RTC)` took over the first one; it gets `noFreeChannel`.
- PIT one-shot timers: `trigger(0)` fired twice; re-triggering from the callback stopped after the first
  callback. A PIT timer stopped at the end of its period could still invoke its callback once.
- GPT: a latched interrupt after releasing a timer could call an empty callback (hard fault); re-triggering a
  one-shot just when the previous delay expired could cancel the new trigger.
- TCK: `getRemainingTime()` returned a value when nothing was pending, and a wrong value for TCK_RTC;
  `setPeriod(p, keepElapsed)` on a stopped timer could invoke the callback.
- FTM timers (Teensy 3): destroyed timers kept running and their channels were never freed; periods were
  longer by the interrupt latency; the first callback could come up to 35 ms late.
- `begin()` with a too long period didn't start the timer.
- Compiler warnings with Teensyduino 1.62.

**Known limitations**

- `TCK_RTC` one-shot delays can be up to one tick (30.5 µs) shorter than requested.
- Frequency literals (`20Hz`) are not available.

## v1.4.3 - 2024-07-22

Released on GitHub only, not in the Arduino Library Manager.

**Fixed**

- Removed a debug output to `Serial` from the GPT `setPeriod()` that was introduced in v1.4.2 (printed on every `begin()` of a periodic GPT timer).

**Known issues**

- PIT periods longer than about 179 µs are still limited to 179 µs (see v1.4.2; fixed in v1.5).

## v1.4.2 - 2024-07-22

Released on GitHub only, not in the Arduino Library Manager.

**Fixed**

- PIT and GPT: the timer clock is now determined in `begin()` instead of at construction time. Periods are now correct when the PIT/GPT clock is switched to the bus clock (`USE_GPT_PIT_150MHz`).

**Known issues**

- Regression: PIT periods longer than about 179 µs are wrongly limited to 179 µs, and a `periodOverflow` error is posted (fixed in v1.5).
- The GPT `setPeriod()` prints a debug value to `Serial` (fixed in v1.4.3).

## v1.4.1 - 2023-08-08

**New**

- Example for the RTC timer module.

**Fixed**

- The RTC timer module is now compiled only for T4.x boards.

## v1.4.0 - 2023-07-26

**New**

- New timer module `RTC` (T4.x only) for `PeriodicTimer`. It generates interrupts synchronized to the real time clock with periods of 1 s, 0.5 s, 0.25 s, ... down to 30.5 µs; other requested periods are rounded up to the next available period. The RTC module provides no `OneShotTimer`.

**Changed**

- Removed a C++17 dependency (inline variables).
- Requires Teensyduino 1.59 or later. Use v1.3.0 for Teensyduino 1.58 and older.

## v1.3.1 - 2023-04-20

**Fixed**

- Compile errors caused by the Arduino `abs` and `round` macros.
- Linker error (multiple definition) caused by a missing `inline` in the TMR channel code.

## v1.3.0 - 2023-01-27

**Fixed**

- Namespace conflict with the bundled `inplace_function.h` when TeensyTimerTool is used together with EncoderTool.

## v1.2.0 - 2022-12-08

**Changed**

- The callback storage now uses `inplace_function` (replaces `staticFunctional::function` from v1.1.0).

**Fixed**

- `config.h` is now included with an explicit path to avoid picking up a `config.h` from another library.

## v1.1.0 - 2022-09-17

**Changed**

- Callbacks are now stored in `staticFunctional::function` instead of the larger `std::function`. The change is transparent for user code; capturing lambdas remain supported.
- `staticFunctional` requires the toolchain of Teensyduino 1.58 or later. Older Teensyduino versions automatically fall back to plain `void(*)()` function pointer callbacks.

**New**

- Example showing the use of capturing lambdas as callbacks.

## v1.0.0 - 2022-09-11

**Changed**

- Compatibility with the gcc 11.3 toolchain introduced with Teensyduino 1.58.
- Breaking: frequency literals (`100_kHz`, `0.25_MHz`, ...) and `frequency.h` were removed because they do not compile with gcc 11.3. Periods must be given in µs or as `std::chrono` durations.
- Examples reworked.

## v0.4.4 - 2022-04-23

**New**

- `getRemainingTime()` returns the time until the next timer event in µs (float). Currently implemented for TCK timers only.

## v0.4.3 - 2022-02-11

- Maintenance release (no functional changes); published to trigger an update in the Arduino Library Manager.

## v0.4.2 - 2022-02-11

**Fixed**

- `start()`, `stop()` and the `OneShotTimer` functions now check whether the timer was initialized (`begin()` called) and post `errorCode::notInitialized` otherwise.

## v0.4.1 - 2021-11-07

**New**

- `setPeriod()` and `setNextPeriod()` for TCK, PIT, GPT and TMR timers. `setPeriod()` changes the running period immediately (if the request comes too late, one period with the old value is completed first); `setNextPeriod()` applies the new period after the current one.
- `setPeriod()` and `setNextPeriod()` accept `std::chrono` durations.
- Float periods for FTM timers.
- PinInfo example extended to the Teensy MicroMod.

## v0.4.0 - 2021-11-04

**New**

- New 64-bit software timer `TCK_RTC` (T4.x), based on the 32.768 kHz real time clock instead of the cycle counter. Useful where the lower drift of the RTC crystal matters.
- `TCK32` and `TCK64` timers available on all supported boards.
- Working destructors for TCK, TMR and GPT timers: a timer going out of scope is stopped and its hardware released, so timers can be used as local variables.

**Changed**

- TCK timers restructured internally.

## v0.3.5 - 2021-08-26

- Maintenance release (no functional changes; corrected version number).

## v0.3.4 - 2021-08-26

**New**

- Float periods for FTM timers.
- PinInfo example extended to the Teensy MicroMod.

## v0.3.3 - 2021-07-10

**New**

- Support for the Teensy MicroMod.
- `setPeriod()` and `setNextPeriod()` for PIT timers.

**Changed**

- Interrupt handlers use a DSB instead of re-reading the interrupt flag (faster).

**Fixed**

- Compile error on Teensy LC caused by a wrong override.

## v0.3.2 - 2020-10-22

**New**

- `getMaxDuration()`: the maximum period as `std::chrono` duration.

**Fixed**

- Wrong value returned by `getMaxPeriod()`.

**Changed**

- Simplified overloads for `std::chrono` durations and frequency literals.

## v0.3.1 - 2020-10-17

**New**

- Frequency literals for `begin()` of periodic timers, e.g. `timer.begin(isr, 100_kHz)` (removed again in v1.0.0).

## v0.3.0 - 2020-10-14

**New**

- `begin()` and `trigger()` accept `std::chrono::duration` values and literals, e.g. `trigger(10ms)` or `begin(cb, 0.5s + 10ms)`.
- Examples restructured; new `std::chrono` and DoubleExposure examples.

## v0.2.1 - 2020-08-26

**New**

- `OneShotTimer::triggerDirect()` with a precalculated reload value from `getTriggerReload()`, avoiding the float conversion on every trigger. Implemented for TCK and GPT. Relevant mainly for boards without FPU (e.g. T3.2).
- Example showing more complex use cases.

## v0.2.0 - 2020-08-02

**New**

- New 64-bit software timer `TCK64` (all T3.x and T4.x boards). Extends the 32-bit cycle counter to 64 bit and allows periods up to 2^32 µs (about 71.6 min).

**Changed**

- `getMaxPeriod()` returns seconds instead of µs.

## v0.1.12 - 2020-06-29

**New**

- `start()` and `stop()` for FTM timers.

## v0.1.11 - 2020-06-29

**New**

- `start()` and `stop()` for TCK, TMR, GPT and PIT timers.
- `begin()` of `PeriodicTimer` takes an optional third parameter; `false` prepares the timer without starting it.

**Changed**

- T3.x: the `YIELD_STANDARD` strategy now uses the EventResponder, as already done for T4.
- `errorCode::OK` is no longer posted to the error handler.

**Fixed**

- Timers could not be stopped.

## v0.1.10 - 2020-06-19

**Changed**

- T4: the `YIELD_STANDARD` strategy hooks into `yield()` via the EventResponder instead of a copy of the core's `yield()` code (slightly less efficient, but independent of core changes).
- Examples no longer use the deprecated `Timer` class.

## v0.1.9 - 2020-05-13

**New**

- FTM prescaler configurable per FTM module.
- Example showing which timers are used for PWM pins.

**Fixed**

- FTM one-shot timers could not be retriggered.
- Wrong GPIO information printed on T3.x boards.

## v0.1.8 - 2020-04-11

**New**

- Board detection prepared for the Teensy 4.1.

**Fixed**

- Teensy 3.2 boards were not identified correctly.

## v0.1.7 - 2020-04-10

**New**

- Compile-time check for the minimum required Teensyduino version (1.50).

**Fixed**

- Only one of the four T4 PIT channels could be allocated.
- Wrong `#define` for the yield strategy.

## v0.1.6 - 2020-04-09

**New**

- PIT timers for the Teensy 4.
- Number of TCK timers configurable.

**Changed**

- Board detection uses the new `ARDUINO_TEENSYxx` defines.

**Fixed**

- Error callback for plain function callbacks.

## v0.1.5 - 2020-04-05

**Changed**

- Default yield strategy changed to `YIELD_STANDARD`.

## v0.1.4 - 2020-04-04

**New**

- Configuration setting for overriding `yield()` (yield strategy).

## v0.1.3 - 2020-03-28

**New**

- Configuration setting for the default FTM prescaler.

## v0.1.2 - 2020-03-03

**Fixed**

- Wrong file name case of a header (build failure on case-sensitive file systems).

## v0.1.1 - 2020-03-02

**Fixed**

- Callback was called twice for very short interrupt handlers (re-reading of the interrupt flag reactivated).

## v0.1.0 - 2020-02-07

**New**

- First tagged release. `PeriodicTimer` and `OneShotTimer` with a common base class, using TCK (software timers on the cycle counter), GPT, TMR (QuadTimer) and FTM timers.
- Float periods; `getMaxPeriod()` returns the longest possible period of a timer.
- Callbacks as function pointers, functors and lambdas, including callbacks with context.
- Error handling with a configurable error callback; warning on period overflow.
- Library configurable from the user sketch (`userConfig.h`): TMR prescaler, GPT/PIT clock source.
