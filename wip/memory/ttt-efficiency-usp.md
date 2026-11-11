---
name: ttt-efficiency-usp
description: "TTT's USP is efficiency ~equal to hand-written register code; check every hot-path change with the perf suite"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 64cfb1fa-2a4a-4273-bbbd-0b6dde6c2b43
  modified: 2026-11-10T12:45:36.647Z
---

Efficiency is the main feature of TeensyTimerTool: using it must cost about the same as hand-written timer register code. luni64 rejected the GPT free-run mode (step 5b) because it tripled the ISR latency, even though it added setNextPeriod.

**Why:** On T4.x every GPT/PIT register access (read or write) costs ~100 CPU cycles at the 24 MHz timer clock (~44 at 150 MHz), because the peripheral bus is synchronized to the 600 MHz core. A TMR access costs ~20 cycles, RAM ~5. A read also waits until all pending writes have landed. Flag clears need a DSB before the ISR returns; without it callbacks run 2–3 times (luni64 has seen this repeatedly; forum thread 73404). Measured: a GPT/PIT flag reset needs ~75–90 cycles depending on the phase to the 24 MHz clock. The DSB adapts to it (84–108 cycles) and never failed. Fixed waits fail at some phases only, so double-callback tests need phase jitter (perf suite phaseJitter).

**How to apply:**
- Keep register reads out of ISRs, trigger() and start(). Keep RAM copies instead and add a comment explaining why.
- Write constant values instead of read-modify-write, but always clear flags with a real register write.
- Run `python harness/host/run_tests.py perf` in TeensyTimerTool-TestSuite (limits relative to hand-written code) for any hot-path change.
- Prefer simple, maintainable designs over complex ones (luni64 picked option A over a hybrid GPT mode).

Related: [[ttt-tdd-and-changelog]], [[ttt-test-system-status]]
