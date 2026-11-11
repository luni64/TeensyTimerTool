---
name: ttt-docs-rewrite-todo
description: TeensyTimerTool docs move from the GitHub wiki to MkDocs/GitHub Pages in docs/ of the library repo (decided 2026-11-10); DOCS_TODO.md is the input list
metadata:
  node_type: memory
  type: project
  originSessionId: 64cfb1fa-2a4a-4273-bbbd-0b6dde6c2b43
  modified: 2026-11-10T20:22:56.039Z
---

Decided 2026-11-10 (luni64): the documentation moves from the GitHub wiki (10 pages, ~9500 words) to a GitHub Pages site,
MkDocs Material like AutoNum / Matrikelhelfer, sources in a `docs/` folder of the library repo (not a separate repo).
Site: https://luni64.github.io/TeensyTimerTool/ (no custom domain: luni64 says the GitHub address is fine for this
audience, unlike AutoNum/Matrikelhelfer). Work on branch `docs` (off `unittests`), skeleton f68db05: mkdocs.yml at the
repo root, workflow .github/workflows/docs.yml (push to master), example code included via pymdownx.snippets from examples/.
Image size (DoubleExposure page: ~520 KB of PNG/JPG) is to be reduced later, luni64 wants to look at that separately.
Pending doc items are in `C:\Users\lutz\source\Teensy\TeensyTimerToolDev\DOCS_TODO.md` (outside the repo); the tested API
behaviour is in TeensyTimerTool-TestSuite/README.md ("API behaviour"); user-visible changes in CHANGELOG_DRAFT.md.

Status 2026-11-11: all chapters written (5b03133, branch docs): Getting started, Usage, Choosing a timer, Callbacks,
Error handling, Configuration, Examples (+ DoubleExposure, images linked from the wiki raw URLs, not copied),
Advanced (Performance, PWM clashes), Reference (5 pages), Upgrading from 1.4. luni64 reviews the complete manual next
(Word export review/*-6.docx). Bugs found while writing (PLAN "Open findings") are fixed after the manual is final.

**Why:** docs versioned with the code (v1.5 changes behaviour), reviewable before release, examples included from source.
**How to apply:** keep adding doc items to DOCS_TODO.md until the rewrite consumes them. Style: see [[ttt-docs-style]].
See also [[ttt-release-policy]], [[ttt-t3x-no-new-features]].
