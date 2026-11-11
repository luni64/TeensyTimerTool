# WIP handover (remove this folder before merging to master)

State of the v1.5 renovation on 2026-11-11, so the work can continue on another machine or in a new
Claude Code conversation. Nothing here is part of the library; `library.json` excludes `wip/`.

**To continue in a new Claude Code conversation:** tell Claude to read `wip/HANDOVER.md` first, then the files in
`wip/memory/` (the notes Claude keeps between conversations: rules, preferences and status).

## Branches (library repo)

| Branch | Content | Pushed |
|---|---|---|
| `master` | 1.4.x release state | yes |
| `bugfixes` | earlier fixes | yes |
| `unittests` | API renovation for 1.5 (tests moved out to TeensyTimerTool-TestSuite) | yes (WIP) |
| `docs` | `unittests` + new MkDocs manual in `docs/` + this folder | yes (WIP) |
| `unittests-presplit` | local backup of the history before the tests were moved out | no (local only) |

## Current task: documentation (branch `docs`)

- **Status:** all chapters written. luni64's thorough review of the complete manual is pending. The terms are
  explained in click popups (Material annotations, `term (1)` + `{ .annotate }`); the floating boxes were removed.
- **Preview:** `pip install mkdocs-material`, then `mkdocs serve` in the repo root
  → http://127.0.0.1:8000/TeensyTimerTool/. Check with `mkdocs build --strict`.
- **Review in Word:**
  - Export: `python wip/tools/docs_to_docx.py [pages]` writes `wip/review/TeensyTimerTool-docs-<date>.docx`
    and never overwrites an existing file. Pandoc must be installed.
  - Read the comments back with `python wip/tools/docx_comments.py <file>`. Don't use pandoc for this: it
    drops comments anchored in code blocks.
  - `wip/review/TeensyTimerTool-docs-2026-11-11-6.docx` is the complete manual without comments. It still
    shows the old boxes instead of the popups.
- **Backlog:** `wip/DOCS_TODO.md`. Style rules: `wip/memory/ttt-docs-style.md`.
- **Later:**
  - Reduce the DoubleExposure image sizes.
  - Point the wiki pages to the new site.
  - Enable GitHub Pages (Settings → Pages → Source: GitHub Actions). The workflow deploys on push to master.

## After the manual is final

- **Fix the open findings** listed in `wip/PLAN_API_v1.5.md` under "Open findings". Work test first (TDD):
  - PIT `setPeriod()` starts a stopped timer.
  - `setNextPeriod()` on a stopped timer behaves differently on PIT than on TMR/TCK.

  The Reference already describes the intended behaviour and marks both findings with HTML comments.
- **Next hardware test round:** run all suites, plus `USE_GPT_PIT_150MHz`, plus the new `HelloOneShot`
  example on a board.
- **Changelog:** user-visible changes go to `wip/CHANGELOG_DRAFT.md`.
- **Release:** a single v1.5 release, with no tags or version bumps before it.

## Not in this repo

- **TeensyTimerTool-TestSuite:** a separate local git repo (harness + suites, `project.toml`). It has no
  remote yet, so it's only on the original machine. Hardware tests need it.
- **Originals of the `wip/` files:** they live in the dev root `TeensyTimerToolDev/` on the original machine:
  - `PLAN_API_v1.5.md`, `CHANGELOG_DRAFT.md`, `DOCS_TODO.md`
  - `tools/`
  - `review/`

  The copies here are from 2026-11-11. Merge changes back when you return to that machine.
