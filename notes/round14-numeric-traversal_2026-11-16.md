# Round 14: numeric traversal holdouts

Tags: #round14 #matching #once-loop #source-evidence #checkpoint4

Date: 2026-11-16. Source baseline: checkpoint three, `f4989797f`.

This bounded review leaves both source units unchanged. Independent stock checks
still match all **1,280 bytes** of `status_screen.c` and all **92 bytes** of
`fn/sub_08221D40.c`. Their compiler-inclusive scan remains **zero errors and three
once-loop warnings**. No ignore or new helper is adopted.

## Two-row numeric display

NumericDisplay_DrawPair and NumericDisplay_DrawStatusValues each wrap the initial
row counter and digit-array base assignment in a one-shot loop. Removing only
that wrapper changes eight bytes in either complete unit. In DrawPair, the digit
array base and background record exchange `ip` and `r8`; the rendered values and
instruction count are unchanged.

The same unit's StatusScreen_DrawClock renders two-row digit tiles through actual
indexed digit and destination arrays. Following that structure in DrawPair gives
the correct 1,280-byte unit length but changes eleven bytes, including an inner
cursor/row-offset register exchange and the order of cursor setup. Fully ordinary
indexed row loops change eighteen bytes. Resetting an actual digit cursor from
the local array removes eight bytes from the unit.

The four repeated two-digit draws also support investigating a complete shared
renderer. Ordinary inline and macro candidates were tried for the full two-row
operation, including fixed-font and row-local cursor forms. None matches: complete
units range from 1,284 to 1,308 bytes. These candidates remain source evidence,
not a reason to hide the existing initialization wrappers inside a private macro.

## Profile array

ScalarApproach_ApplyProfileArray wraps only the value-pointer increment. Ordinary
deletion leaves the 92-byte instruction sequence intact but exchanges the value
cursor and count between `r5` and `r6`, changing nine bytes. Scope reports the
value pointer at nine weighted references across 38 live instructions, priority
7,105; count has ten references across 36 instructions, priority 8,333. A
diagnostic priority increase of 1,229 for the value pointer reproduces the ROM,
confirming this decision. It is not used in any stock check or source change.

Actual indexed traversal, separate loads with incrementing cursors, a distinct
derived coefficient, and a shared whole-array inline were tested. They produce
84–104 bytes. An ordinary cursor `for` loop retains the same nine-byte gap.

The Hresvelgr copy at `0819B488` initially looked like useful independent source
evidence: it uses the same profile operation and two ordinary traversal cursors,
and transplanting that body also matches all 92 bytes of `08221D40`. Its history
undercuts that inference. Commit `1b5c7f432` copied this function with its assembly
barrier; commit `30f392ca7` then introduced those argument-copy cursors while
removing the barrier. The sibling therefore does not independently establish the
original source structure. **That exact alias-only draft is deliberately
withheld**, rather than reported as a warning fix.

## Replayable evidence

`evidence/round14-numeric-traversal-candidates_2026-11-16.jsonl` preserves all
**18 complete source drafts**: seventeen mismatches and the one withheld exact
draft. Every record contains the baseline and candidate hashes, a replayable
source patch, hypothesis, stock result, and disposition. All eighteen patches
replay and hash-verify against the named baseline. They are independent
reconstructions, not a patch series; no generated ROM or assembly is included.

The three source warnings stay visible. The coordinator owns the combined
checkpoint counts and full ROM build.
