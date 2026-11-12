# TeensyTimerTool – documentation to-do

Collected while fixing bugs on branch `bugfixes` (lib/TeensyTimerTool). To be worked into the
documentation rewrite. Commit references are from that branch.

## setPrescaler() (new / now working) — 1821669, 29ded8d

- `timer.setPrescaler(psc)` now works (fixes issue #32, supersedes PR #31). Was a linker error before.
- Supported timers and values:
  - **TMR (QUAD)**: `PSC_1` … `PSC_128` (150 MHz / 2^psc), per channel. Default from `TMR_DEFAULT_PSC`.
  - **GPT**: `PSC_1` … `PSC_4096` (perclk / 2^psc). Default 1. New constants `PSC_256` … `PSC_4096` (GPT only).
  - **PIT, TCK, RTC**: not supported → `errorCode::notImplemented`.
  - **FTM (T3.x)**: unchanged, compile-time only via `FTM_DEFAULT_PSC`.
- Out-of-range values (incl. `PSC_AUTO`) → `errorCode::argument` (new error text "Invalid argument").
- Can be called before or after `begin()`:
  - before: stored and applied in `begin()`; a failure is returned by `begin()` (timer still starts).
  - after (timer running): takes effect immediately. The reload stays in ticks, i.e. the current
    period scales with the prescaler until `setPeriod()` / `setNextPeriod()` is called.
- Max periods (getMaxPeriod(), seconds):
  - TMR: ÷1 437 µs … ÷128 55.9 ms
  - GPT @24 MHz: ÷1 179 s … ÷4096 733 008 s (8.5 days); @150 MHz (`USE_GPT_PIT_150MHz`): 28.6 s … 32.6 h
  - PIT: 179 s @24 MHz, 28.6 s @150 MHz (no prescaler)
- Resolution: one timer tick = 2^psc / clock. Periods are rounded to the nearest tick, minimum one tick.
  Worth a table "prescaler vs. resolution vs. max period" per timer.

## Behaviour changes worth mentioning

- Periods are now rounded to the nearest tick (were truncated) for PIT, GPT, TMR — 8ebc3cd, 29ded8d.
- TMR periods were one tick too long (0.85 µs at default PSC_128) — fixed in 8ebc3cd.
- PIT periods > ~179 µs were clamped to 179 µs in v1.4.2 / 1.4.3 (GitHub only, never in the
  Arduino Library Manager) — fixed in 3b573d1. Changelog item.
- `begin()`, `setPeriod()`, `setNextPeriod()`, `trigger()` now *return* `errorCode::periodOverflow`
  for PIT and GPT (TMR already did); the period is set to the maximum.
- `TMR_DEFAULT_PSC` outside `PSC_1 … PSC_128` is now a compile error (`static_assert`), was masked to 3 bits.
- Destroying a PIT or RTC timer object (local timer, timer in a container…) crashed the board
  (delete on a statically allocated channel). Fixed on branch `unittests`; destroyed timers now stop.
- A stopped TMR channel still got callbacks whenever another channel of the same TMR module fired.
  Fixed on branch `unittests`.
- New: on-target test suite in the separate repo TeensyTimerTool-TestSuite (see its README.md) — mention in the contributor docs.
- The verified API behaviour (formerly test/API_CONTRACT.md in the library) is documented in `TeensyTimerTool-TestSuite/README.md`,
  section "API behaviour" (branch `unittests`) — main input for the rewrite.
- Wiki pages that don't match the code:
  - Basic-Usage: `t1.begin(..., 20Hz)` — frequency literals are deactivated (`frequency.h.deactivated`),
    doesn't compile (backlog item). "Changing the period: Explanation to be done" → setPeriod /
    setNextPeriod / PeriodMode semantics are now defined (see TeensyTimerTool-TestSuite/README.md).
  - Error-Handling: uses `beginOneShot()` (deprecated `Timer` API) on `OneShotTimer`, and FTM (T3.x
    only) in all examples.
  - getMaxPeriod example: "TMR(PSC_AUTO)" — TMR doesn't support PSC_AUTO (now a compile error).
  - Home/README: "supports T3.X and T4.0" (T4.1, MicroMod missing); README mentions Teensyduino 1.59beta.
  - Supported-Timers: TCK_RTC listed for T3.6 (T4.x only); RTC missing in the Home overview; TCK64
    listed for T-LC (only TCK there).
  - Configuration: typos `USE_GPT_PID_150MHz`, `defautConfig.h`; says NR_OF_TCK_TIMERS "will be
    settable later" (it is).
  - Callbacks: says `std::function`, it is `stdext::inplace_function` with a 32 byte capture limit.
  - `TCK_RTC` one-shot delays are up to one tick (30.5 µs) *shorter* than requested (`trigger(500)` →
  459 … 489 µs), it counts whole 32 kHz ticks from wherever the trigger falls within a tick. Measured
  with the LA (`la_one_shot`, branch `unittests`). Document, or round up by one tick in the code?
- Delay trigger() → callback: delay rounded to ticks + 0.5 … 1.5 µs on the hardware timers (LA measured).
- Capability table per timer changed in step 5: getRemainingTime PIT/GPT/TMR, keepElapsed PIT.
  GPT: no setNextPeriod / keepElapsed (restart mode, step 5d), RTC lacks them (and one-shot).
- Efficiency page: library vs hand-written register code (perf suite numbers, step 5d). Note that every
  GPT/PIT register access costs ~100 CPU cycles at 24 MHz (~44 with `USE_GPT_PIT_150MHz`), TMR ~20:
  TMR has the lowest latency; why the GPT has no setNextPeriod (free-run ISR costs ~4x).
- Document `getPeriod()`, `getResolution()` and the chrono getters (`getPeriod<microseconds>()` …),
  incl. "latest period set" semantics after `setNextPeriod()` (a887ccb). Good place for the resolution
  table per timer / prescaler.
- Document the edge cases (step 3, 14b5119): period 0 / negative values, one-shot stop()/start(),
  trigger(0), re-trigger from the callback, warning `prescalerIgnored` (see TeensyTimerTool-TestSuite/README.md).
- Document: timers are move-only (no copies), can be kept in a std::vector (d25060b).
- Undocumented so far: `end()`, `getRemainingTime()` (TCK only), `PeriodMode`, `begin()` on a
    running timer, re-`trigger()`, error codes for all the edge cases (see TeensyTimerTool-TestSuite/README.md).
- Fixed by the `api` suite (branch `unittests`), all user visible:
  - RTC periods now round up to the next longer 2^-n s as documented (1 ms gave 976.6 µs).
  - `begin(cb, p, false)` on TMR started the timer anyway.
  - TCK, TCK64, TCK_RTC and RTC now return `periodOverflow` for too long periods.
  - TCK timers round periods to the nearest tick (truncated before).
  - `getRemainingTime()` for TCK_RTC returned a wrong value (unit).
  - Second `PeriodicTimer(RTC)` now gets `noFreeChannel` instead of silently overwriting the first.
  - `setPeriod()` / `setNextPeriod()` before `begin()` → `notInitialized` (crashed).
  - `end()` implemented: stops the timer, releases the channel (was `notImplemented`).

## Config system (defaultConfig.h / userConfig.h)

- Build matrix (TestSuite harness/host/build_matrix.py, 2026-11-09): all config variants build without warnings on
  T4.1/4.0/MicroMod/3.6/3.5/3.2/3.0/LC; `userConfig.h` files copied from v1.4.1 and v1.3.1 still work.
- `PLAIN_VANILLA_CALLBACKS`: only plain functions and captureless lambdas — capturing lambdas and
  functors don't compile (examples ArrayOfTimers, CapturingLambdas, DoubleExposure). Say so in the docs.
- Without `USE_TIME_LITERALS` the examples RTC, UsingChronoDurations1/2, CallbackWithParams don't compile.

- No new config constants: existing `userConfig.h` files stay compatible.
- Document that `TMR_DEFAULT_PSC` is only the default; runtime change via `setPrescaler()`.
- `USE_GPT_PIT_150MHz` affects GPT and PIT (shared perclk) and possibly `IntervalTimer`.

## Examples

- New example `02_Advanced/Prescaler` (TMR and GPT, before/while running, max period, invalid value);
  verified on a T4.1 with the LA (10.000 / 10.24 µs, LED 500.053 ms = 2930 ticks at PSC_4096). Link it from the docs.
- `03_Applications/DoubleExposure` uses `FTM0` → T3.x only; say so in its README (it doesn't build for T4.x).
- All T4.x examples verified on a T4.1 with GCC 15 (2026-11-09). Comment/output fixes in the examples
  themselves are done (MoreTimers, RTC, RTC_TCK_Timer, CallbackWithParams, CapturingLambdas).
- Explain timer resolution in the docs: e.g. MoreTimers' TMR1 "100 µs" gives 99.84 µs with the default
  PSC_128 (tick 0.853 µs); RTC periods snap to the next longer 2^-n s (25 ms → 31.25 ms).

## setPeriod() semantics changed (branch `unittests`) — document prominently, possibly breaking

- `setPeriod(p)` now **restarts** the period on all timers (new period starts at the call). That was
  already the behaviour of PIT, GPT and RTC; TMR and TCK timers used to count the elapsed time.
- Old TMR/TCK behaviour: `setPeriod(p, PeriodMode::keepElapsed)` (TMR, TCK, TCK64, TCK_RTC; others
  return `notImplemented`). Mention in the changelog / migration notes.
- `begin()` on a running timer restarts it with the new callback and period (now part of the contract).
- RTC: periods are synced to the clock, so a "restart" can't be exact there.

## Backlog / feature requests (not docs, collected here for now)

- `start()` after `stop()` should **continue** the interrupted period instead of restarting it
  (all modules restart today), plus a separate reset function to restart the period. luni64 (docs review
  2026-11-11): "would be better to just continue with the rest of the period and have a reset function" →
  check feasibility per module (remaining ticks at stop(); PIT/GPT/TMR/TCK), possibly breaking.
- Getters return seconds, the setters take microseconds: consider returning microseconds from the getters
  (luni64, docs review 2026-11-11). Breaking change → needs an entry under "Breaking" and a migration note.
- Go through the complete list of error codes and improve the names (luni64, docs review 2026-11-12,
  comment on `errorCode::argument`). The old names can stay as aliases so that existing code keeps
  compiling, but only the new names are documented. Known candidate: `errorCode::reload` (period 0) →
  something understandable for beginners, e.g. `errorCode::invalidPeriod` (docs review 2026-11-11).
- Frequency literals (`20Hz`, `frequency.h.deactivated`): were homebrew and didn't work reliably.
  Redo properly or drop; the wiki still uses them (Basic-Usage, start/stop example).
- Keep the `Timer` class as deprecated (no removal in v1.5).
- `triggerDirect()` / `getTriggerReload()`: now implemented for PIT, GPT, TMR, TCK (and FTM on T3.x), tested in
  `api_trigger_direct` (246b00f). Document as advanced API: precomputed ticks for chained one-shots; uint64_t
  reload for TCK64 / TCK_RTC.
- `getRemainingTime()`: done for PIT, GPT, TMR (443e99c), see CHANGELOG_DRAFT.

## Release

- Bump `version=` in library.properties (> 1.4.1), otherwise the Arduino indexer skips the release
  (v1.4.2 and 1.4.3 were skipped for this reason).
- Double callbacks (forum thread 73404, T4 beta 2019): the library's ISRs end with a DSB, so even empty
  callbacks are called exactly once. Measured 2026-11-10: GPT/PIT need ~75–90 CPU cycles after the flag reset
  (depends on the phase to the 24 MHz clock), the DSB waits 84–108 cycles (adapts), reading the flag back
  ~160–210. Worth a note for users who write their own ISRs (attachInterruptVector).
- CPU clock: TMR (and GPT/PIT with `USE_GPT_PIT_150MHz`) run on F_BUS = F_CPU / 1..4 (max. 150 MHz, 816 MHz → 204).
  The clock is read in `begin()` / `setPrescaler()`; after `set_arm_clock()` at runtime call one of them again.
  Max. periods / resolutions in the tables are for 600 MHz.
