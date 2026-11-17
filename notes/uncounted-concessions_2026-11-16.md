# Steering C the concession scanner does not count (first sweep)

Tags: #concessions #phase3 #phase4 #tooling

`tools/concessions.py` reports no concession on `main` after round 13. That figure is
scanner-defined. This sweep looked for C that exists only to steer the compiler and that the
scanner's regexes do not see. It is a first pass with text detectors; nothing here changes the
score yet.

## Round 13

33 of the 36 function commits name allocation-only forms in their own messages
(`notes/uncounted-concessions/round13-commits.tsv`). The forms, worst first:

* **Unreachable arms with dead code.** A guard that can never be true (`(s32)bound < 0` where
  `bound` came from a `u16`) around a call, load or store. The arm survives until jump2 and
  changes copy preferences or reload's scratch set on the way. Examples: the dead
  `Video_SetRenderCallbacks` call in `sub_08217430`, two dead `Sound_PlaySong` calls in
  `Skeleton_UpdateDefeat`, a dead `Mod` call in `WirelessGameMenu_InitEquipmentChoices`.
* **Known-zero terms.** `(u32)gRandomTable & ~((u16)(u32)input >> 16)`,
  `table | (((u32)burst & 31) >> 5)`, `i + ((u16)i >> 16)`. They hide a constant or an
  induction variable from an early pass and fold away in combine.
* **Single-pass loops with a body.** `do { x = y; } while (0);` raises a reference weight. The
  scanner counts only the empty form.
* **Storage unions and reused locals** that couple two unrelated values into one pseudo.
* **Dead reads and stores** that keep a pseudo canonical across a CSE boundary.

A pin states what it does. These forms read like game logic and are not. They should count.

## The whole corpus

The same forms were in the tree before round 13, written by earlier workers. Counts on `main`
after round 13, outside `src/lib` (`notes/uncounted-concessions/sites.tsv`: kind, file, line,
text):

| Kind | Sites | Files | Detector | Precision on a sample |
|---|---:|---:|---|---|
| once-loop | 154 | 111 | a `do { ... } while (0)` with a body, outside a macro | high |
| dead-read | 172 | 139 | an expression statement with no effect (`pad14;`, `v;`, `actor->base.body.position;`) | high; 86 are `pad*;` and need one look |
| zero-shift | 21 | 13 | a value shifted right by at least its width, or masked then shifted past the mask | high |
| never-negative | 31 | 22 | `x < 0` where `x` is declared unsigned in the file | medium (type lookup is per file) |
| volatile-view | 72 | 46 | a pointer-to-volatile local or cast the scanner's `volatile` kind skips | medium; hardware registers are legitimate |
| local-union | 431 | 208 | a union declared inside a function | low; fixed-point and byte-view unions are ordinary C |

The first five kinds touch 302 files. In addition 183 comments in 137 files talk about
allocation, reload, priorities or "to match" (`notes/uncounted-concessions/comments.tsv`); a
sample is about half real admissions and half ordinary uses of the words.

## What text detectors miss

The round 13 dead arms are mostly invisible to regexes: the guard is `(s32)bound < 0` with
`bound` a `u32` copy of a narrow value. Two text checks were tried:

* A callee named in a function's C but never referenced by its compiled code: 16 hits in 13
  functions, all worth a look, but it misses every dead call whose callee is also called for real.
* More call sites in C than `bl` instructions: 332 hits, nearly all cross-jumping. Not usable.

The reliable detector is in the compiler. Every RTL insn carries its source line. A source
line that produces a call, a memory store or a conditional jump at RTL generation and has no
insn left at final emission is C that compiles to nothing. scope's instrumented agbcc can
report those lines per function. Inline helpers with constant arguments will produce some
legitimate hits; those take a `concession-ok`.

## Proposed changes

1. Scanner kinds, scored and excusable: `once-loop`, `dead-read`, `known-zero` (zero-shift),
   `never-true` (never-negative), and widen `volatile` to pointer-to-volatile outside register
   access. Tracked only: `local-union`.
2. A `dead-code` kind fed by the compiler-side report above, since the scanner alone cannot see it.
3. A review of the 183 comments and of the round 13 commit list, adding sites by hand where
   neither detector fires (storage unions, reused locals).

With 1 alone the concession-free share drops by a few hundred functions. That is the honest
figure for phase 3.
