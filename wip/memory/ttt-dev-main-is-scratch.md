---
name: ttt-dev-main-is-scratch
description: "In TeensyTimerToolDev, src/main.cpp is a scratch test sketch — overwrite freely, no backup needed"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 64cfb1fa-2a4a-4273-bbbd-0b6dde6c2b43
  modified: 2026-11-09T16:53:48.835Z
---

In the TeensyTimerToolDev project, `src/main.cpp` exists only for testing/improving the TeensyTimerTool library (in `lib/TeensyTimerTool`, its own git repo). Overwrite it with test sketches freely; don't back it up or restore it.

**Why:** User said the whole purpose of the project is testing the library.
**How to apply:** Write test sketches straight into src/main.cpp; still note it must be recompiled (copying an old file back keeps an old mtime and make skips it).
