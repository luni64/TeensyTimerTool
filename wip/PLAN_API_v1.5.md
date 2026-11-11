# TeensyTimerTool v1.5 – API contract improvements (plan for review)

Status: **reviewed 2026-11-10**, decisions below are resolved; implementation step by step. Branch: `unittests` (lib/TeensyTimerTool).
Related: `CHANGELOG_DRAFT.md` (user visible changes), `DOCS_TODO.md` (docs rewrite), `TeensyTimerTool-TestSuite/README.md`
(API behaviour = the contract).

## Approach (test driven)

Every step follows the same cycle:

1. **Contract**: write the new / changed behaviour into `TeensyTimerTool-TestSuite/README.md`, section "API behaviour"
   (marked as planned until the step is done).
2. **Tests first**: add the tests (api suite, la suite where pins matter) and run them against the current
   code. They must fail (red), otherwise they don't test the change. Record the red result in the commit
   message.
3. **Implement** until the new tests pass (green).
4. **Regression**: all on-target suites, build matrix subset (`-b teensy41 teensy36 -V default
   plain_vanilla_callbacks`), full matrix at the end of each step that changes headers.
5. **Track**: add the user visible changes to `CHANGELOG_DRAFT.md`, docs items to `DOCS_TODO.md`.
6. **Commit** (one or two commits per step; no push without asking).

Test placement rules: tests that may crash the board on the current code go to the end of the suite;
compile-time properties (e.g. "not copyable") are checked with type traits at runtime (`check(...)`), so a
failing property is a failed check, not a broken build.

Each step can be reviewed on its own; decisions marked **[decide]** need your answer before the step starts.

---

## Step 1 – Timer objects: no copies, moves allowed (bug, crash)

**Done** 2026-11-10, d25060b (red: copyable, leaked channels, crash in api_move on GPT1 → green).

**Problem**: `BaseTimer` owns a channel through a raw pointer and is copyable. A copy of a started timer
(`PeriodicTimer b = a;`, `std::vector<PeriodicTimer>` growing) releases the channel twice → crash.

**Contract**
- Timers can't be copied (compile error).
- Timers can be moved (move constructor / move assignment). The moved-to timer takes over the channel and
  keeps running; the moved-from timer is empty (`start()` etc. return `notInitialized`, `begin()` allocates
  a new channel). Move-assigning to a timer that owns a channel releases that channel first.
- Timers can be stored in `std::vector` (`emplace_back`, `push_back(std::move(t))`, growing).

**Tests (api)**
- `api_not_copyable`: `std::is_copy_constructible` / `is_copy_assignable` are false, `is_move_*` true
  (both timer classes).
- `api_move` (end of suite): per type: begin, move-construct, callbacks continue with the same period; the
  moved-from object returns `notInitialized`; destroy both → no crash, channel free (all channels can be
  allocated again). Move assignment onto a running timer releases its old channel.
- `api_timers_in_vector`: 10 TCK + 4 TMR1 timers in a growing `std::vector`, all keep running, clear() frees
  all channels.

Breaking? Only for code that copies timers, which crashed already (copying a not yet started timer
worked — it becomes a compile error; it's a move then, or needs a reference). → changelog "breaking".

## Step 2 – Errors are posted exactly once (bug)

**Done** 2026-11-10, d59ba4a (red: 11 double posts → green). Not changed: the GPT channel destructor posts
`GTP_err` for an unknown module (internal, can't happen) and the FTM channel (T3.x) posts `periodOverflow`
from its tick conversion — both are posted once. FTM: `begin()` / `trigger()` still return `OK` after
posting the overflow (T3.x, contract differs) → fix when the T3.x capability table exists.

**Problem**: some channels post errors themselves (`ITimerChannel` defaults) and the API layer posts the
result again → the error function is called twice, differently per timer type.

**Contract**: every API call that returns a code ≠ `OK` calls the error function exactly once with that
code; calls returning `OK` don't call it. Functions returning a value (`getMaxPeriod`, `getRemainingTime`,
`getPeriod`) post once when they fail.

**Tests (api)**
- `api_error_posted_once`: per type, a table of failing calls (unsupported `setNextPeriod`,
  `keepElapsed`, `setPrescaler`, `getRemainingTime`, overflowing `begin` / `setPeriod` / `trigger`, calls
  before `begin()`, invalid prescaler, `noFreeChannel`): error function called once, with the returned code.

**Implementation**: channels only return codes; `postError` only in the API layer (`BaseTimer`,
`PeriodicTimer`, `OneShotTimer`). Also the T3.x channels (FTM, TCK) — they share the interface, no new
features there.

## Step 3 – Undefined edge cases get a defined behaviour (contract)

**Done** 2026-11-10, 14b5119. Red: setPeriod(0) hung the board, OneShotTimer::start() fired the callback on
all timers, PIT trigger(0) fired twice, PIT chained one-shot (re-trigger from the callback, new test
api_trigger_from_callback) fired only once → green. la_one_shot_stop was already green (stop() did cancel),
mutation checked.

**Contract** (agreed)
| Call | Today | Proposed |
|---|---|---|
| `OneShotTimer::stop()` while a trigger is pending | undefined | cancels it, returns `OK` |
| `OneShotTimer::stop()` idle | undefined | `OK`, no effect |
| `OneShotTimer::start()` | undefined (may start the channel) | `notImplemented`, no effect (use `trigger()`) |
| `trigger(0)` | undefined | callback as soon as possible (one tick), `OK` |
| `trigger(< 0)` | undefined | `argument`, no callback |
| `begin(cb, < 0)`, `setPeriod(< 0)`, `setNextPeriod(< 0)` | undefined | `argument`, timer unchanged |
| `setPeriod(0)`, `setNextPeriod(0)` | undefined | `reload` (like `begin(cb, 0)`), timer unchanged |
| `begin()` on a timer without prescaler after `setPrescaler()` (before begin) | timer runs, returns the *error* `notImplemented` | timer runs, returns the new *warning* `prescalerIgnored` (negative code) |
| `begin()` after `setPrescaler()` with an invalid value (before begin) | timer runs, returns `argument` | unchanged: `argument` is a programming error and stays an error (timer runs with the previous prescaler) |

**Tests**
- api: `api_one_shot_stop`, `api_one_shot_start`, `api_trigger_zero`, `api_negative_values`,
  `api_period_zero` (extended), `api_set_prescaler` (expectations changed to the warning).
- la: `la_one_shot_stop` — trigger + stop, no edge on the callback pin.

## Step 4 – Read back the actual period: `getPeriod()`, `getResolution()` (new)

**Done** 2026-11-10, a887ccb. Red with the API declared and channels returning NAN → green. Deviation from the
draft: `getPeriod()` returns the period set by the *latest* call (after `setNextPeriod()` immediately
the next period), not "the period in effect" — same on all timers and read from the hardware registers.
Chrono variants return `zero()` on errors and clamp to `duration::max()` (TCK64 max. period in ns).

**Problem**: periods are rounded per timer (RTC 2⁻ⁿ s, TMR 0.85 µs at PSC_128, GPT 171 µs at PSC_4096),
the user can't see the result.

**Contract** (agreed)
- `PeriodicTimer::getPeriod()`: the period in effect, after rounding. After `setNextPeriod()` it changes
  when the new period starts.
- `getResolution()` (both timer classes): one tick of the timer (with the current prescaler).
- Units: **seconds as float**, like `getMaxPeriod()` / `getRemainingTime()` (consistent getters; the
  setters keep µs / chrono).
- **chrono variants** for all four getters (`getPeriod`, `getResolution`, `getMaxPeriod`,
  `getRemainingTime`), selected by a template argument with a float default, so existing code is
  unaffected:
  ```cpp
  float s = t.getPeriod();                      // seconds, as before
  auto us = t.getPeriod<microseconds>();        // std::chrono::microseconds
  auto ns = t.getRemainingTime<nanoseconds>();
  ```
  Integer durations are rounded to the nearest unit (`std::chrono::round`), floating point durations
  (`duration<float, std::micro>`) are exact. On errors (before `begin()`, not supported) the chrono
  variants return `T::zero()` and post the error (chrono has no NAN). Available with
  `USE_TIME_LITERALS` (default), like the chrono setters.
- Before `begin()`: `NAN` + `notInitialized` (like the other getters).

**Tests**
- api: `api_get_period`: per type and several periods (incl. RTC rounding, prescalers): `getPeriod()`
  equals the expected rounded value and the measured mean period; `getResolution()` equals the table tick.
  `api_queries_before_begin` extended.
- api: `api_chrono_getters`: per type the chrono variants of all four getters agree with the float
  versions (µs, ns, `duration<float>`), rounding of integer durations, `zero()` + posted error before
  `begin()`. Build matrix: compiles without `USE_TIME_LITERALS` (chrono variants absent).
- la: `la_periodic` additionally compares the pin period with `getPeriod()`.

## Step 5 – Close feature gaps the hardware allows (new features, T4.x)

**Done** 2026-11-10 (443e99c, 8735c87, 6279c61). GPT switched to free-run mode for 5b; along the way: the GPT
ISR called a null callback for interrupts latched after a release (hard fault), TCK getRemainingTime()
returned times with nothing pending. Not yet checked on hardware: `USE_GPT_PIT_150MHz` (build only).

Each sub step: flip the flag in the capability table of `api.cpp` (and `la.cpp`) first → the existing
generic tests fail (red) → implement → green.

| Sub step | Timer | Feature | Implementation idea |
|---|---|---|---|
| 5a ✓ 443e99c | PIT, GPT, TMR | `getRemainingTime()` | read the counter (PIT CVAL, GPT CNT, TMR CNTR) |
| 5b ✗ 8735c87 | GPT | `setNextPeriod()` | free-run mode, OCR1 += reload in the ISR — **reverted** (step 5d) |
| 5c ✓ 6279c61 | PIT (GPT reverted) | `setPeriod(p, PeriodMode::keepElapsed)` | elapsed from the counter, load the remainder, then the period |

Additional tests: one-shot `getRemainingTime()` (pending / after firing / never triggered) for all types
that support it — contract (agreed): time to the pending callback, 0 if none pending.
la: `la_next_period` runs automatically for GPT once the flag is set.

Risks: 5b changes the GPT operating mode → all GPT tests (gpt suite: compare register, prescaler, one-shot)
must stay green; `USE_GPT_PIT_150MHz` variant checked on hardware once.

## Step 5d – Efficiency (added 2026-11-10 after review of step 5) — **Done** ebfacc7

**Problem**: the free-run GPT of 5b tripled the interrupt latency (153 → 460 ns) and quadrupled the ISR cost
(124 → 528 cycles); the PIT ISR read all four TFLG registers per interrupt (since master, ~530 cycles); 5c
added register reads to the PIT ISR / `trigger()`. Measured: every GPT/PIT register access costs ~100 CPU
cycles at 24 MHz (~44 at 150 MHz; the peripheral bus clock is synchronized to the 600 MHz core), TMR ~20, RAM ~5.

**Decision** (luni64): GPT back to restart mode (option A; the hybrid restart/free-run was too involved).
GPT `setNextPeriod()` / `keepElapsed` → `notImplemented` again. Rule: no register reads in ISRs / trigger /
start, RAM copies where needed, comments explaining it; flag clears stay register writes + DSB.

**Tests first**: new `perf` suite: hand-written register ISRs as reference in the same firmware, limits
relative to them (latency, cost per interrupt, trigger(), trigger → callback) + exact callback counts
(double callbacks from unseen flag clears). Red: 8 of 8 → green. Mutation: GPT ISR without DSB → 3x callbacks
detected.

**Result** (latency / cost per interrupt; reference = hand-written): GPT 160 ns / 124 cyc (ref 153 / 126),
PIT 220 / 224 (ref 153 / 124; the TFLG read of the shared interrupt), TMR 140 / 88 (ref 113 / 68).
trigger(): GPT 340 ns (ref 355), PIT 225 (142), TMR 188 (115). GPT one-shot ISR reads the status once
(stale interrupt after a re-trigger would cancel the new trigger; the alternative DSB in trigger() costs 0.55 µs).
Not done (option): PIT single channel shortcut without TFLG read (would lose the stale interrupt detection).
Decided (luni64): keep as is (IntervalTimer is likely slower).

**Follow-up** 246b00f: faster trigger() (no float division, conversion overlaps the first register write, final
channel classes), triggerDirect()/getTriggerReload() for PIT and TMR. trigger(): PIT 220 ns (ref 148), TMR 150 (120),
GPT 290 (345). TCK_RTC early one-shots: tried +1 tick (never short, but trigger(0) up to 2 ticks), reverted;
stays a known limitation (luni64: only if easy).
**Next test round** (luni64): run all suites once with `USE_GPT_PIT_150MHz` on hardware.

## Step 6 – `PSC_AUTO` for TMR and GPT — **postponed** to a later release (luni64, 2026-11-10)

**Idea**: `setPrescaler(PSC_AUTO)` → the timer picks the finest prescaler that fits the requested period
at every `begin()` / `setPeriod()` / `trigger()`. Timer agnostic: "just make this period work, as
precisely as possible".
Open questions **[decide]**: `setNextPeriod()` with a period that needs another prescaler (TMR applies a
prescaler change immediately → can't be deferred exactly); does `getResolution()` then change with each
period? Proposal: decide after step 4/5, when `getPeriod()` / `getResolution()` exist.

## Open findings (from writing the reference, 2026-11-11) — fix after the manual is finished (luni64)

- **Bug:** `setPeriod()` on a *stopped* PIT timer starts it (PITChannel::setPeriod does stop(); …; start()).
  TMR, GPT and TCK stay stopped. Needs a test (api: setPeriod on a stopped timer → no callbacks, start()
  runs with the new period) → fix.
- **Contract gap:** `setNextPeriod()` on a stopped timer: PIT uses the new period from start() on, TMR and
  TCK run one old period first. Define the behaviour (proposal: first period after start() uses the new
  value, as for setPeriod), test, align.

## Not in this plan (backlog, see DOCS_TODO.md)

- `start()` continuing the interrupted period instead of restarting, plus a reset function (luni64, docs
  review 2026-11-11: check feasibility)
- getters in microseconds instead of seconds (consistent with the setters; breaking, luni64 docs review 2026-11-11)
- rename `errorCode::reload` (period 0) to a beginner friendly name, keep the old one as alias
- frequency literals; `triggerDirect()` / `getTriggerReload()` (raw ticks, `uint32_t`/`uint64_t`)
- unit cleanup of the getters (seconds vs. µs) beyond the new getters
- T3.x capability tables for the api suite (hardware tests on T3.x)

## Order and effort (rough)

| Step | Kind | Breaking | Effort |
|---|---|---|---|
| 1 copy/move | bug | copying unstarted timers no longer compiles | small |
| 2 errors once | bug | no | small |
| 3 edge cases | contract | return codes change in undefined cases | small–medium |
| 4 getPeriod / getResolution | new API | no | small–medium |
| 5a–c feature gaps | new features | no | medium each |
| 6 PSC_AUTO | new feature | no | medium, optional |
