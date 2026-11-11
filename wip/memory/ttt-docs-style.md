---
name: ttt-docs-style
description: "TeensyTimerTool docs style - professional, technical-manual register that stays accessible; beginner info first, expert details later or in an advanced section"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 64cfb1fa-2a4a-4273-bbbd-0b6dde6c2b43
  modified: 2026-11-11T17:47:28.463Z
---

TeensyTimerTool documentation: readers range from newbies to C++ specialists. Write in a professional,
neutral technical-manual register that stays accessible to beginners (explain terms like "callback" once,
short complete examples). Not chatty: luni64 found the first draft (2026-11-10) "written for teenagers"
("That's all you need", "Good to know", "nice", "just fine", "you won't notice") ; reworded in e111c4d
(luni64: tone fine now, 2026-11-11). The original wiki aimed at the friendly PJRC forum tone; friendly = clear and helpful,
not colloquial.

**Why:** beginners get frustrated if they don't find what they need early in a chapter; specialists want
the technical material without it getting in the beginners' way, and a too casual tone reads unprofessional.

**How to apply:** each chapter starts with what most readers need (purpose, minimal working example, common
pitfalls), then a "Details" part after a separator (rounding, latency, edge cases, error codes), and
technical background (registers, efficiency, ISR internals, C++ techniques) in the Advanced section.
Declarative sentences ("`stop()` cancels a pending trigger"), no exclamations or filler. luni64 likes the
general setup: chapters, per-chapter TOC, intro/details split. Related: [[ttt-docs-rewrite-todo]].

Conventions from the first review (2026-11-11): introduce every new term before or where it first appears,
using a click popup = Material annotation: `term (1)` in the paragraph, `{ .annotate }` on the next line,
then `1.  **Term**` + indented explanation (question-mark icon in docs/stylesheets/extra.css). luni64 rejected
floating "concept" boxes (2026-11-11: disturb the text flow; the right strip stays for the TOC) and called
the popups "beautiful";
don't present TTT as better than Paul's IntervalTimer ("in addition to"); numbers like ticks or latencies
come with how they are calculated / why they exist; prefer real code examples over prose descriptions.
Review workflow: `python tools/docs_to_docx.py [pages]` (dev root) -> review/*.docx, luni64 comments in Word,
read back with `python tools/docx_comments.py review/<file>.docx` (reads the docx XML; pandoc silently drops
comments anchored in code blocks — 5 of 27 comments were lost that way in the first review).
