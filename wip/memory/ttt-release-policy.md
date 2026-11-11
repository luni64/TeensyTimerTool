---
name: ttt-release-policy
description: "TeensyTimerTool release plan — one v1.5 release after the renovation; push branches freely, never tag or bump version before that"
metadata:
  node_type: memory
  type: project
  originSessionId: 64cfb1fa-2a4a-4273-bbbd-0b6dde6c2b43
  modified: 2026-11-09T18:01:46.146Z
---

Decided 2026-11-09: TeensyTimerTool renovation (branch `bugfixes`, fixes since v1.4.1 + unit tests + docs rewrite) ships as a single **v1.5** release in the Arduino Library Manager. User switches machines often, so branches get pushed regularly.

**Why:** The Arduino indexer picks up any git tag whose library.properties `version=` is new; user doesn't want intermediate versions in the manager.
**How to apply:** Pushing branches/commits is fine (after confirming). Don't create or push tags, and don't bump `version=` in library.properties / library.json until the user starts the v1.5 release. Unit tests live in the library repo under `test/` (Python runner), see [[ttt-docs-rewrite-todo]].
