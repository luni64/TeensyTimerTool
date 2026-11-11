# Configuration

TeensyTimerTool reads its settings from a configuration file at compile time: which timers form the timer
pool, the default prescalers, the clock of the GPT and PIT timers, how the software timers are called,
and the callback type. Most sketches work with the default settings. This chapter shows where the
settings are stored, how to change them for one sketch, and what each setting does.

## Where the settings are stored

The default settings are in the file `src/defaultConfig.h` of the library. There are two ways to change
them:

- **For one sketch (recommended):** copy `defaultConfig.h` into the sketch folder and rename the copy
  to `userConfig.h`. If the library finds a `userConfig.h`, it uses this file instead of
  `defaultConfig.h`. The settings apply to this sketch only and survive library updates.
- **For all sketches:** edit `defaultConfig.h` in the library folder. The change is lost when the
  library is updated or reinstalled.

Whether the library finds a `userConfig.h` in the sketch folder depends on the build system:

=== "PlatformIO"

    PlatformIO finds `userConfig.h` without further settings. After copying the file, PlatformIO
    rebuilds the project.

=== "VisualTeensy"

    VisualTeensy finds `userConfig.h` without further settings. After copying the file, run
    *Build → Clean* and build again.

=== "Arduino IDE"

    The Arduino IDE compiles libraries without the sketch folder in the include path, so the library
    doesn't find `userConfig.h`. A `platform.local.txt` file can add the sketch folder:

    1. Locate the folder containing `platform.txt` of the Teensy package, e.g. on Windows
       `%LOCALAPPDATA%\Arduino15\packages\teensy\hardware\avr\<version>\`.
    2. Create a file `platform.local.txt` in this folder.
    3. Copy the line starting with `recipe.cpp.o.pattern=` from `platform.txt` into it, and insert
       `-I{build.path}/sketch` before `"{source_file}"`.

    For Teensyduino 1.62, the line reads:

    ```
    recipe.cpp.o.pattern="{compiler.path}{build.toolchain}{build.command.g++}" -c {build.flags.optimize} {build.flags.common} {build.flags.dep} {build.flags.cpp} {build.flags.cpu} {build.flags.defs} -DARDUINO={runtime.ide.version} -DARDUINO_{build.board} -DF_CPU={build.fcpu} -D{build.usbtype} -DLAYOUT_{build.keylayout} "-I{build.path}/pch" {includes} -I{build.path}/sketch "{source_file}" -o "{object_file}"
    ```

    Restart the IDE afterwards. `platform.local.txt` affects all sketches, but has no effect on
    sketches without a `userConfig.h`. After an update of Teensyduino, the file has to be created
    again in the folder of the new version. To deactivate it, delete or rename the file.

!!! info "Configuration files of older versions"
    A `userConfig.h` copied from an older version of the library (v1.3, v1.4) still works with v1.5:
    no settings were added or removed.

## Settings

### Timer pool

```cpp
#elif defined(ARDUINO_TEENSY41)
    TimerGenerator* const timerPool[] = {GPT1, GPT2, TMR1, TMR2, TMR3, TMR4, TCK};
```

The pool is defined separately for each board. A timer declared without a timer type
(`PeriodicTimer t1;`) takes the first free channel, from left to right: with the default setting for
Teensy 4.1, the first two timers use GPT1 and GPT2, the next sixteen the channels of TMR1 … TMR4, and the
following ones TCK timers. Timer types can be removed, added or reordered, e.g. to keep the TMR timers
free for `analogWrite()` or to use the TCK timers first.

### Default prescaler of the TMR timers (Teensy 4)

```cpp
constexpr int TMR_DEFAULT_PSC = PSC_128;
```

The prescaler of all TMR channels after `begin()`: `PSC_1` … `PSC_128`. The default `PSC_128` gives a
resolution of 0.853 µs and a maximum period of 55.9 ms; `PSC_1` gives 6.67 ns and 437 µs. The prescaler
of a single timer can be changed at runtime with `setPrescaler()`, see
[Prescaler](choosing-a-timer.md#prescaler).

### Prescalers of the FTM timers (Teensy 3)

```cpp
constexpr int FTM_DEFAULT_PSC[] = {/*FTM0*/ PSC_AUTO, /*FTM1*/ PSC_AUTO, /*FTM2*/ PSC_AUTO, /*FTM3*/ PSC_AUTO};
```

One prescaler per FTM module: `PSC_1` … `PSC_128`, or `PSC_AUTO`, which selects a prescaler giving
about 0.5 µs per tick at the board's bus clock. The FTM prescalers can only be set here, not at runtime.

### Clock of the GPT and PIT timers (Teensy 4)

```cpp
constexpr bool USE_GPT_PIT_150MHz = false;
```

`false` (default): the GPT and PIT timers are clocked with 24 MHz (resolution 41.7 ns, maximum period
179 s). `true`: they are clocked with the peripheral clock of 150 MHz (resolution 6.67 ns, maximum
period 28.6 s). The setting applies to both timer types together, and it also changes the clock of the
`IntervalTimer` of the Teensy core.

### Number of software timers

```cpp
constexpr unsigned NR_OF_TCK_TIMERS = 20;
```

The maximum number of tick timers (TCK, TCK64 and TCK_RTC together) in use at the same time.

### How the software timers are called

```cpp
#define YIELD_TYPE YIELD_STANDARD
```

The software timers are evaluated by the function `TeensyTimerTool::tick()`, which has to be called as
often as possible. This setting selects how this happens:

- `YIELD_STANDARD` (default): the library adds the call to the standard `yield()` function of the
  Teensy core, which also handles `serialEvent()` and the `EventResponder`.
- `YIELD_OPTIMIZED`: the library replaces `yield()` with a function that only calls `tick()`. This
  reduces the latency of the software timers. Recommended if the sketch doesn't use `serialEvent()` or
  the `EventResponder`, which no longer work with this setting.
- `YIELD_NONE`: the library doesn't change `yield()`. The sketch has to call `TeensyTimerTool::tick()`
  itself, as often as possible.

### Callback type

```cpp
//   #define PLAIN_VANILLA_CALLBACKS
```

When defined, callbacks are plain function pointers (`void (*)()`) instead of the default
`inplace_function`. Plain functions and lambda expressions without captures still work; capturing lambdas
and functors don't compile. See [Callbacks](callbacks.md#the-callback-type).

### Time units

```cpp
    #define USE_TIME_LITERALS
```

Enables periods and delays as `std::chrono` durations (`250ms`, `1.5s` …) and the getters with units.
When removed, periods and delays are given in microseconds only.

### Advanced features

```cpp
//   #define ENABLE_ADVANCED_FEATURES
```

Reserved; currently without effect.
