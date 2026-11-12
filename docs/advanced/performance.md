# Performance

TeensyTimerTool is designed to add as little overhead as possible to the timer hardware. Its interrupt
handlers and `trigger()` functions avoid reading timer registers, which are slow to access on the
Teensy 4 (about 100 CPU cycles each for GPT and PIT), and use the same register writes a hand-written
implementation would use.

## Comparison with hand-written code

*Hand-written register code* means the minimal code for one fixed timer channel, programmed directly
with the registers of the processor: an interrupt handler that resets the interrupt flag and calls the
callback through a function pointer. It serves exactly one channel, so it needs neither the common
interface nor a search for the channel that fired, and it supports plain function callbacks only. It is
the lower limit of what a timer interrupt costs.

The following values were measured on a Teensy 4.1 at 600 MHz, compared to such hand-written code doing
the same job in the same firmware (reference values in brackets):

| Timer | Latency (timer event → callback) | CPU time per interrupt | Duration of `trigger()` | `trigger()` → callback, minus the delay |
|---|---|---|---|---|
| GPT | 160 ns (153 ns) | 126 cycles (124) | 290 ns (345 ns) | 1.05 µs (0.91 µs) |
| PIT | 220 ns (153 ns) | 226 cycles (127) | 220 ns (148 ns) | 0.89 µs (0.67 µs) |
| TMR | 140 ns (113 ns) | 88 cycles (68) | 150 ns (120 ns) | 0.30 µs (0.26 µs) |

- **Latency:** time from the timer event to the first instruction of the callback.
- **CPU time per interrupt:** the processor time the interrupt takes from the sketch, with an empty
  callback.
- **`trigger()` → callback:** the time from calling `trigger(delay)` to the start of the callback, minus
  `delay`. It includes the duration of `trigger()` and the latency.

The PIT channels share one interrupt; the library reads one status register to find the channel that
fired, which accounts for most of the difference to the reference. The TMR timers have the lowest
latency, since their registers are accessed considerably faster than those of the GPT and PIT.

### Measurement method

The values were measured on the board itself, with the counters of the timers and the cycle counter of
the processor. The hand-written reference uses a second module of the same timer type (GPT2, PIT
channel 3, TMR2) in the same firmware: an interrupt handler that resets the flag, calls the callback
through a function pointer and ends with a `dsb` instruction, as the library does. The latency is read
from the counter of the timer at the start of the callback; for this measurement, GPT and PIT run with
150 MHz to get a resolution of 6.7 ns. The CPU time per interrupt is measured by the loss of iterations
of a busy loop at a 10 µs period.

## Software timers (TCK)

The TCK timers are evaluated in `yield()`. The time between the due time and the callback depends on how
often `yield()` runs: a few µs if `loop()` runs freely, more if the sketch blocks. `delay()` calls
`yield()` while waiting. With the default setting `YIELD_STANDARD`, `yield()` also handles the serial
events of the Teensy core; `YIELD_OPTIMIZED` reduces the latency of the TCK timers
(see [Configuration](../configuration.md#how-the-software-timers-are-called)).

If the sketch contains code that blocks for a long time, e.g. a loop working through a large amount of
data, the TCK callbacks are delayed until this code has finished. A call to `yield()` in the body of
such a loop keeps the TCK timers responsive:

```cpp
for (unsigned i = 0; i < nrOfSamples; i++)
{
    process(samples[i]); // long calculation
    yield();             // gives the TCK timers the chance to invoke their callbacks
}
```

## Precomputed reload values

`trigger(delay)` converts the delay into timer ticks before it starts the timer. Sketches that trigger a
one-shot timer with the same delay many times, e.g. to generate pulse sequences by re-triggering from
the callback, can compute the tick value once with `getTriggerReload()` and start the timer with
`triggerDirect()`:

```cpp
#include "TeensyTimerTool.h"
using namespace TeensyTimerTool;

OneShotTimer t1(TMR1);
uint32_t reload; // 20 µs in timer ticks

void setup()
{
    pinMode(0, OUTPUT);
    t1.begin([] {
        digitalToggleFast(0);
        t1.triggerDirect(reload); // re-trigger without conversion: square wave with 20 µs per half period
    });
    t1.setPrescaler(PSC_1);
    t1.getTriggerReload(20, &reload);
    t1.triggerDirect(reload);
}

void loop()
{
}
```

On Teensy 4, `triggerDirect()` saves little compared to `trigger()`: the conversion runs while the
processor waits for the first register write, so both take about the same time (TMR 148 ns vs. 150 ns).
The values in the table above are the main limit for the shortest delays and the accuracy of chained
one-shots. `triggerDirect()` and `getTriggerReload()` are supported by the PIT, GPT, TMR, TCK and FTM
timers; the reload of the TCK64 and TCK_RTC timers is a `uint64_t`.

## Double callbacks

The registers of the timers are connected to the processor through a slower peripheral bus. A write to a
register, e.g. the reset of an interrupt flag, takes some time to reach the timer, while the processor
continues. If an interrupt handler returns before the flag reset has arrived, the timer still requests
the interrupt, and the handler runs a second time. With very short callbacks, the callback would be
invoked twice or three times per period.

The interrupt handlers of TeensyTimerTool end with a `dsb` (data synchronization barrier) instruction,
which waits until all pending writes have reached the peripherals. On a Teensy 4 at 600 MHz, the flag
reset of a GPT or PIT timer needs 75 … 90 CPU cycles to arrive, depending on the phase of the 24 MHz
timer clock; the `dsb` waits exactly as long as necessary.

The same applies to sketches with their own interrupt handlers (`attachInterruptVector()`): reset the
flag, and end the handler with `asm volatile("dsb");`.

## Changing the CPU clock at runtime

The TMR timers, and the GPT and PIT timers with `USE_GPT_PIT_150MHz`, are clocked with the peripheral
bus clock F_BUS, which follows the CPU clock: F_BUS = F_CPU / 1 … 4, at most 150 MHz (528 MHz → 132 MHz,
816 MHz → 204 MHz). `begin()` and `setPrescaler()` read the current bus clock. After changing the CPU
clock at runtime with `set_arm_clock()`, call `begin()` or `setPrescaler()` again, otherwise the timers
run with periods calculated for the previous clock. The maximum periods and resolutions in this manual
apply to the default 600 MHz.
