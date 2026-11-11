---
name: ttt-tdd-and-changelog
description: "TTT API work is test driven (tests first, must fail red), planned in PLAN_API_v1.5.md, every user visible change goes into CHANGELOG_DRAFT.md"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 64cfb1fa-2a4a-4273-bbbd-0b6dde6c2b43
  modified: 2026-11-10T07:08:28.340Z
---

For TeensyTimerTool API changes: write the tests first and make sure they fail on the current code, then implement.
Larger work gets a written plan reviewed by the user before implementing, then done in steps
(plan: TeensyTimerToolDev/PLAN_API_v1.5.md, written 2026-11-10, status draft for review).
Every user visible change is tracked in TeensyTimerToolDev/CHANGELOG_DRAFT.md (baseline v1.4.1) in the same step.

**Why:** user wants to use the new test system test-driven, review plans before big changes, and needs a change history for v1.5.
**How to apply:** per step: contract in TeensyTimerTool-TestSuite/README.md → red tests → implement → green + regression → changelog/DOCS_TODO → commit (library and TestSuite repo separately). See [[ttt-test-system-status]], [[ttt-docs-rewrite-todo]], [[ttt-release-policy]].
