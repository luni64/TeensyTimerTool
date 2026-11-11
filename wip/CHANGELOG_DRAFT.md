# TeensyTimerTool – change history (draft)

Collects every user visible change for the v1.5 release notes / a future CHANGELOG.md.
Baseline: **v1.4.1**, the last version in the Arduino Library Manager (v1.4.2 and v1.4.3 were tagged on
GitHub only; their changes are included here). Commit ids refer to branches `bugfixes` / `unittests`.

Rule: every commit with a user visible change adds its line here in the same step (see PLAN_API_v1.5.md).
Sections: Breaking, New, Changed, Fixed, Docs/examples, Internal. "(planned)" = in the plan, not done yet.

## v1.5.0 (unreleased)

### Breaking / possibly breaking
- `setPeriod(p)` now restarts the period on **all** timers (the new period starts at the call). TMR and TCK
  timers used to keep the elapsed time; get the old behaviour with
  `setPeriod(p, PeriodMode::keepElapsed)`. (6cbdde2)
- `begin(cb, p, false)` on TMR timers no longer starts the timer (it did, against the documentation). (e2fe263)
- `TMR_DEFAULT_PSC` outside `PSC_1 … PSC_128` is a compile error (`static_assert`); it was silently masked
  to 3 bits. (29ded8d)
- Periods are rounded to the nearest timer tick (were truncated) on PIT, GPT, TMR and TCK timers; RTC
  periods are rounded up to the next 2⁻ⁿ s as documented (1 ms gave 976.6 µs). (8ebc3cd, 29ded8d, e2fe263)
- Timer objects can't be copied any more: a copy of a started timer released its channel twice and
  crashed (e.g. `std::vector<PeriodicTimer>` growing). Code copying a not yet started timer now gets a
  compile error; use a reference or `std::move`. (d25060b)
- Return codes for so far undefined cases: negative periods / delays → `argument`, `setPeriod(0)` /
  `setNextPeriod(0)` → `reload` (the timer is unchanged in both cases); `OneShotTimer::start()` →
  `notImplemented` (it fired the callback). (14b5119)
- `begin()` on a timer without prescaler after `setPrescaler()` returns the new warning
  `prescalerIgnored` instead of the error `notImplemented` (the timer runs in both cases). (14b5119)

### New
- `setPrescaler(psc)` for TMR (`PSC_1 … PSC_128`, per channel) and GPT (`PSC_1 … PSC_4096`) timers, before
  or after `begin()`. New constants `PSC_256 … PSC_4096` (GPT only). Fixes #32, supersedes PR #31.
  (1821669, 29ded8d)
- Timers can be moved (move constructor / assignment) and kept in containers like `std::vector`; the
  moved-from timer is empty. (d25060b)
- `PeriodMode` for `setPeriod()`: `PeriodMode::restart` (default) and `PeriodMode::keepElapsed`
  (TMR, TCK, TCK64, TCK_RTC). (6cbdde2)
- `end()` implemented: stops the timer and releases its channel; `begin()` afterwards allocates a new one
  (returned `notImplemented` before). (e2fe263)
- New example `02_Advanced/Prescaler`. (eb679f3)
- New example `02_Advanced/TimersInContainers`: timers in a `std::vector`, added and removed at runtime over
  the serial monitor (works without extra hardware, prints the toggles per pin). (7586ded)
- On-target test suite, logic analyzer tests and build matrix: separate repository TeensyTimerTool-TestSuite
  (not part of the library download).
- `OneShotTimer::stop()` cancels a pending trigger (now defined and tested). (14b5119)
- New warning code `errorCode::prescalerIgnored`. (14b5119)
- `PeriodicTimer::getPeriod()`: the period actually used, i.e. the latest period set, rounded to timer
  ticks (TMR 100 µs → 99.84 µs, RTC 1 ms → 1.95 ms). `getResolution()`: one timer tick. T4.x timers. (a887ccb)
- All getters (`getPeriod`, `getResolution`, `getMaxPeriod`, `getRemainingTime`) optionally return a
  `std::chrono::duration`: `t.getPeriod<microseconds>()`. Without template argument they return float
  seconds as before. (a887ccb)
- `getRemainingTime()` for PIT, GPT and TMR timers (was TCK only). (443e99c)
- `setPeriod(p, PeriodMode::keepElapsed)` for PIT timers (was TMR / TCK only). (6279c61)
- `triggerDirect(reload)` / `getTriggerReload(delay, &reload)` for PIT and TMR timers (was GPT, TCK, FTM only):
  trigger with precomputed ticks, e.g. for waveforms of chained one-shots. (246b00f)
  (GPT `setNextPeriod()` / `keepElapsed` were added in 8735c87 / 6279c61 and removed again in ebfacc7: they need
  the free-run mode whose ISR costs ~4x; GPT returns `notImplemented` as before.)

### Changed
- `begin()`, `setPeriod()`, `setNextPeriod()`, `trigger()` return the warning `periodOverflow` for too long
  periods on PIT, GPT, TCK, TCK64, TCK_RTC and RTC (TMR did already) and use the max. period. (8ebc3cd, e2fe263)
- `setPeriod()` / `setNextPeriod()` before `begin()` return `notInitialized` (crashed before). (e2fe263)
- A second `PeriodicTimer(RTC)` gets `noFreeChannel` instead of silently taking over the first one. (e2fe263)
- `begin()` on a running timer restarts it with the new callback and period (now defined behaviour).
- Lower interrupt overhead (measured against hand-written register code, `perf` suite). PIT ISR: only
  channels in use are checked, ~530 → ~220 CPU cycles per interrupt (hand-written: ~125); PIT `trigger()`.
  TMR ISR and `trigger()`: no read-modify-write of registers (latency 193 → 140 ns, hand-written 113 ns).
  GPT unchanged (equal to hand-written code). (ebfacc7)
- Faster `trigger()`: PIT 260 → 220 ns, TMR 193 → 150 ns, GPT 350 → 290 ns (hand-written: 148 / 120 / 345 ns).
  No float division in the µs → ticks conversion, which now runs while the CPU waits for the first register
  write. The channel classes (`PITChannel`, `GptChannel`, `TMRChannel`) are `final`. (246b00f)
- New error text "Invalid argument" for `errorCode::argument`. (29ded8d)
- The error function (`attachErrFunc`) is called exactly once per failed call. It was called twice for
  `setPrescaler()` on PIT/TCK/RTC timers, `setNextPeriod()` on RTC and `begin()` after an unsupported
  `setPrescaler()`. (d59ba4a)

### Fixed
- `begin()` with a too long period returned the warning `periodOverflow` but didn't start the timer; it now
  runs with the max. period (as documented for v1.5). (7586ded)
- FTM timers (T3.x): a destroyed or `end()`ed timer kept running and calling its callback, and its channel was
  never freed (the module ran out of channels); periodic timers were longer by the interrupt latency every
  period; the first callback after `begin()` / `start()` could come up to 35 ms late; periods are rounded to
  the nearest tick (were truncated) and too long ones return `periodOverflow` (was posted by the timer and
  `OK` returned). Found by the api suite, which now also runs on T3.6 and T3.2. (517d8e5)
- PIT periods > ~179 µs were clamped to 179 µs (v1.4.2 / v1.4.3, GitHub only). (3b573d1)
- PIT timers with `USE_GPT_PIT_150MHz` (v1.4.2, c8ef9cc).
- GPT: too long periods were clamped to 179 µs instead of the max. period; periods < 1 tick wrapped
  to the max. (29ded8d)
- TMR periods were one tick too long (0.85 µs at the default `PSC_128`). (8ebc3cd)
- TMR periods were wrong with CPU clocks other than 600 MHz: the TMR runs on F_BUS, which follows
  F_CPU (528 MHz → 132 MHz: periods 13.6 % too long; 816 MHz → 204 MHz: 26 % too short). With
  `USE_GPT_PIT_150MHz`, GPT/PIT periods were slightly off for F_BUS values that aren't whole MHz. (a936d0f)
- `setPrescaler()` didn't link (linker error). (1821669)
- Destroying a started PIT or RTC timer object (local timer, timer in a container …) crashed the board.
  Destroyed timers now stop and free their channel. (693ac22)
- GPT: a latched interrupt after a timer was released could call a null callback (hard fault, board
  disconnects); releasing a timer clears the pending interrupt, the ISR checks the callback. (8735c87, ebfacc7)
- GPT one-shot: re-triggering just when the previous trigger expired could cancel the new trigger (the
  pending interrupt of the old one stopped the timer). `stop()` discards a pending interrupt. (ebfacc7)
- `setPeriod(0)` on a running timer hung the board (interrupt storm). (14b5119)
- PIT one-shot timers: `trigger(0)` fired twice, and a one-shot re-triggered from its own callback
  (chained delays) was switched off after the first callback. (14b5119)
- A stopped TMR channel's callback was called whenever another channel of the same TMR module fired. (dcccc0a)
- PIT: a timer stopped just when its period ended could still call its callback once after `stop()`. (ebfacc7)
- `getRemainingTime()` of TCK_RTC timers returned a wrong value (unit). (e2fe263)
- `getRemainingTime()` of TCK timers returned a time although nothing was pending (stopped timer,
  one-shot never triggered / fired / stopped); now 0. (443e99c)
- Compiler warnings with GCC 15 / Teensyduino 1.62.

### Docs / examples
- Example comments and Serial output fixed (MoreTimers, RTC, RTC_TCK_Timer, CallbackWithParams,
  CapturingLambdas). (5aa182f)
- All T4.x examples verified on a T4.1 with Teensyduino 1.62 / GCC 15.
- Documentation rewrite: see DOCS_TODO.md.

### Known limitations (to mention in the release notes)
- `TCK_RTC` one-shot delays are up to one tick (30.5 µs) shorter than requested.
- Frequency literals (`20Hz`) are still deactivated.
