---
name: ttt-test-system-status
description: "TeensyTimerTool tests live in the separate local repo TeensyTimerTool-TestSuite (split 2026-11-10); layout, board/LA setup"
metadata:
  node_type: memory
  type: project
  originSessionId: 64cfb1fa-2a4a-4273-bbbd-0b6dde6c2b43
  modified: 2026-11-10T18:16:08.500Z
---

Since 2026-11-10 the tests are a separate git repo `TeensyTimerToolDev/TeensyTimerTool-TestSuite` (local only, branch `main`,
not pushed: luni64 wants to polish the history first). The library branch `unittests` was rewritten without test/ (backup
branch `unittests-presplit` = old history incl. tests). Neither is pushed.

Layout: `harness/` is generic (firmware: th_unit.h / th_la.h, namespace `th`, `th::la`; host: run_tests.py, build_matrix.py,
project.py, teensyduino.py, arduino_cli.py, la.py). Project specifics: `project.toml` (library path, suites, includes,
variant base/output), `matrix.toml` ("lib:" sketch patterns = library examples), `suites/`, `common/ttt_measure.h`
(namespace `ttt`). Run from the TestSuite root: `python harness/host/run_tests.py [suites] -v`, `python harness/host/build_matrix.py`.
Decision (luni64): keep the harness inside this repo; split it into its own repo (submodule) when the next library gets tests.

Local config.toml (untracked): `[library] path = "../lib/TeensyTimerTool"`, targets t41 (8077220, COM6, la = true, default),
t36 (2504210, COM4), t31 (T3.1/3.2 board teensy31, 327000, COM5); `--all-targets` runs the hardware suites on t41 and `api` on all
([suite_boards] in project.toml). LA `ppm = 143`. If T3 ports say "access denied": TyCommander's serial monitor holds them, ask
luni64 to close it (happened 2026-11-10).
LA wiring: pin0..4 → D0..D4, pin13 → D5 (test `la_wiring` detects swaps). Default build backend teensyduino (arduino-cli optional, slow).

**Why:** wiring and connected boards change between sessions without notice.
**How to apply:** if LA results look odd run `python harness/host/run_tests.py la -v` first; library commits no longer contain
tests — test changes are committed in the TestSuite repo. See [[ttt-release-policy]], [[ttt-tdd-and-changelog]].
