# Scalar approach and period-256 angle helpers

The reviewed helper family occupies `08221800–082221E8`. The original
`08221800` C function is exactly 52 bytes, ending at `08221834`; its old
2,536-byte progress extent also covered 31 independent helpers. The hidden
tail is 2,484 bytes. Its independent returns, branch targets and literal pools
establish the new boundaries; correcting the inventory must precede counting
the recovered helpers. Nothing here establishes runtime coverage or a
particular game subsystem.

Twenty-five newly recovered helpers account for 2,016 bytes: 780 ordinary C bytes
and 1,236 bytes with minimized register-allocation concessions. Every promoted
function passed its entire independently reviewed extent. Fourteen affected TUs
retain empty operand barriers or local register constraints; none contains
instruction bodies or an explicit r7 binding. The shared
`include/scalar_approach.h` contains ordinary C only. The four existing scalar
seeds at `21774`, `21798`, `217CC` and `21800` now use the same explicit wrapping helpers;
their original full 36/52/52/52-byte extents stay exact without extra credit.

## Contracts

Angles have period 256. Unwrapping chooses the representative relative to the
target with a delta in `[-127, 128]`; the +128 tie remains positive. The signed
halfword delta and explicit narrowing matter to the original code generation.

Half-step helpers snap when within one unit. Quarter-, eighth- and sixteenth-
step helpers force a minimum step of one when their nonzero difference would
otherwise truncate to zero. Intermediate subtraction, addition and negation
use explicit u32 arithmetic before signed interpretation. This preserves the
ROM's wrapping behavior, including negation of INT_MIN, rather than relying
on signed C overflow. The divisor-based helper retains its original `Div` call, signed rounding
and minimum-step branches; divisor validity belongs to the called routine.

Profile indices are clamped only above 15. The ROM coefficient table at
`08614D4C` contains 16 signed halfwords beginning 0, 128, 85 and ending 16.
Negative indices are not silently clamped or declared impossible: the shared
lookup uses integer ROM-address arithmetic to preserve access to preceding
mapped ROM. A valid mapped, aligned address remains a caller requirement.

The profile helpers divide the coefficient by 256 **before** multiplying the
difference. Thus profile zero snaps to the target, while the other built-in
positive profiles produce a zero factor. This surprising order is present in
the machine code and is deliberately retained.

Array helpers read and write signed halfwords, advance both cursors by two
bytes, and narrow results on each store. They preserve target reloads and
store order; no `restrict` or non-overlap claim is introduced. Their loop
counter decrements with defined wrapping and then tests the signed result.
For normal nonnegative counts this processes exactly that many elements;
negative counts are not given an invented universal no-op contract, because
INT_MIN wraps positive on the decrement. Callers must provide valid storage
for the actual number of iterations.

## Reviewed extents

`C` means exact ordinary C; `H` means exact with a minimized concession;
`ASM` remains outside exact C progress. Byte sizes include each function's
alignment and literal pool where present.

| Address | Bytes | State | Observed role |
|---|---:|---|---|
| 08221800 | 52 | Existing C | Scalar sixteenth-step seed |
| 08221834 | 40 | C | Unwrap period-256 angle |
| 0822185C | 20 | C | Centered clamp |
| 08221870 | 56 | C | Centered angle clamp |
| 082218A8 | 32 | ASM | Linear scalar step |
| 082218C8 | 64 | C | Scalar profile |
| 08221908 | 68 | C | Half angle step |
| 0822194C | 84 | C | Quarter angle step |
| 082219A0 | 84 | C | Eighth angle step |
| 082219F4 | 84 | C | Sixteenth angle step |
| 08221A48 | 112 | C | Divisor-based angle step |
| 08221AB8 | 68 | ASM | Linear angle step |
| 08221AFC | 96 | C | Angle profile |
| 08221B5C | 64 | H | Half-step array |
| 08221B9C | 76 | H | Quarter-step array |
| 08221BE8 | 76 | H | Eighth-step array |
| 08221C34 | 76 | H | Sixteenth-step array |
| 08221C80 | 72 | C | Angle unwrap array |
| 08221CC8 | 60 | ASM | Centered clamp array |
| 08221D04 | 60 | ASM | Linear scalar array |
| 08221D40 | 92 | H | Scalar profile array |
| 08221D9C | 100 | H | Half-angle array |
| 08221E00 | 112 | H | Quarter-angle array |
| 08221E70 | 112 | H | Eighth-angle array |
| 08221EE0 | 116 | H | Sixteenth-angle array |
| 08221F54 | 152 | ASM | Timed angle-array helper, tentative |
| 08221FEC | 96 | ASM | Linear angle array |
| 0822204C | 72 | H | Snap/step array, shift 1 |
| 08222094 | 72 | H | Snap/step array, shift 2 |
| 082220DC | 72 | H | Snap/step array, shift 3 |
| 08222124 | 72 | H | Snap/step array, shift 4 |
| 0822216C | 124 | H | Angle profile array |

The six remaining hidden entries total 468 bytes. Their private candidates
are useful allocation references, not published exact C. In particular,
`082218A8` and `08221AB8` retain a commutative ADD operand-order difference.
No BL references to the hidden entries were found in the reviewed code scan;
indirect callers remain possible, so this is not evidence that they are dead.

## Local audit and resumption evidence

The isolated worker's ignored `build/qc-08221834-boundaries.json` records all
31 starts, ends, returns, pools and CFG checks; all entries have no unexplained
nonzero bytes or CFG errors. Matching per-function disassemblies are in
`build/qc-math-<address>.txt`. Proposed build/progress rows are frozen in
`build/qc-08221834-{build,progress}-proposal.csv`; the integrator owns the
actual metadata changes. The 52-byte seed proof is
`build/qc-08221800-52.json`.

Hint minimization used the normal `tools/depin.py --write` implementation
through a private wrapper supplying the frozen reviewed extent to each
`check.py` invocation; stale generated starts must not set these bounds.
The final profile-array proofs are `build/check/run-m8k7a1df` (92 bytes) and
`build/check/run-gzj6ddji` (124 bytes). Both retained one empty `+r(cursor)`
barrier after minimization. A fixed r5 binding instead delayed the first
cursor copy, leaving a four-byte mismatch; allowing natural allocation with
the barrier produced the exact prefix without a fixed register.

The adjacent middle-ROM grouping audit found no analogous unexplained raw
tails in `081915E0–0819321C`, `0819594C–08196A5C` or
`08196BA4–081975C8`. The raw tails within `081976E4–0819804C` were already
covered by five independent metadata entries, all rechecked over their full
76/56/64/56/60-byte extents. These findings support separate owner/state,
animation/palette-task and key-trigger groups, rather than treating the
entire numeric run as one subsystem.


The final five full-extent checks areD9C100(run-2v0zpew_),E00112(run-fybloa42),
E70112(run-5kon9g7a),EE0116(run-6u9iddsj),B5C64(run-2gx83cy8). Depinning
reduces each angle array from20 to9 hints and the half array from9 to5. Every
hard-register value is initialized from an actual load or arithmetic result;
there are no handwritten instruction bodies or explicit r7 bindings.

## Addendum from codex/contiguous-section-sol-resume (2026-10-15, commit d48fe39e)

### Final verification addendum, October 15

A bounded follow-up recovered five more functions from the same reviewed
family. This supersedes the earlier 20-function snapshot and the earlier ASM
status of the five entries listed in this addendum. The final new-helper total is **25 functions /2,016
bytes**, comprising **780 ordinary C bytes and 1,236 concession bytes**.
The additional 504 bytes contribute 0.0209 percentage points against the
2,415,354-byte code denominator, before any independent QC corrections.
The six remaining ASM entries total 468 bytes: `082218A8`, `08221AB8`,
`08221CC8`, `08221D04`, `08221F54` and `08221FEC`.

| Address | Bytes | Commit | Full-span proof artifact |
| 08221D9C | 100 | 0c1f187c | build/check/run-2v0zpew_ |
| 08221E00 | 112 | d3e0b742 | build/check/run-fybloa42 |
| 08221E70 | 112 | e582f9fe | build/check/run-5kon9g7a |
| 08221EE0 | 116 | 8d070381 | build/check/run-6u9iddsj |
| 08221B5C | 64 | 8fa779e2 | build/check/run-2gx83cy8 |

The four contiguous angle arrays retain nine hints each after reduction from
20; the half-step array retains five after reduction from nine. These are
all concession-category functions. Their shared helper header is unchanged.
The angle arrays preserve a masked unsigned intermediate, signed halfword
representative, wrapping subtraction, signed rounding and narrowing stores.
Every operand supplied to an empty asm is initialized; register constraints
reuse a register only after the preceding value's last use. No explicit r7
binding or instruction body is introduced.

The decisive allocation change was to retain the masked intermediate in a
low register while letting the target load use another register, then give
the signed step its own short lifetime. This fixed the signed-load scratch
rotation and all subsequent operand roles without changing the arithmetic.
The half-step array similarly needed the target and result lifetimes kept
separate. All five proofs were repeated after formatting and minimization.
Combined ROM validation remains the integrator's responsibility.
