---
name: ttt-t3x-no-new-features
description: "TeensyTimerTool — don't add new features for T3.x boards (FTM etc.) unless explicitly asked"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 64cfb1fa-2a4a-4273-bbbd-0b6dde6c2b43
  modified: 2026-11-09T17:20:27.152Z
---

Don't bring new features (e.g. runtime prescaler for FTM) to Teensy 3.x / LC in TeensyTimerTool unless the user explicitly asks.

**Why:** User says T3.x boards are being phased out.
**How to apply:** Focus fixes/features on T4.x (PIT, GPT, TMR, TCK, RTC). T3.x: fix bugs / wrong behaviour found by the api suite on T3.6/T3.2 (agreed 2026-11-10), but missing features stay notImplemented (api capability table marks them).
