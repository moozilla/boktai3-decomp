# Matching the second-largest function and removing the duplicate actor layouts

Tags: #matching #compiler #tooling #monsters #skeleton #golem

## Result

`Skeleton_ReactToContact` at `080C12FC` matches its entire 6,372-byte ROM interval,
`080C12FC–080C2BE0` (end exclusive). The C is active in
`src/fn/sub_080C12FC.c`; its guarded draft and assembly fallback are removed.
The expanded target has no register pins, assembly barriers, inline assembly,
or diagnostic compiler settings. Its machine-code SHA-256 is
`76662af024762cf67a4de03e85035c65154bd2d0edaf9ddccc70c2a587afeaba`.

This is the function originally requested as the second-largest unmatched
function, provisionally named `Golem_ReactToContact`. `Skeleton_Init`
(`080C3260`) installs it at callback slot `+0x268`; the AD03 actor registry and
GCL character name establish the Skeleton family. `ReactToContact` remains a
descriptive reconstruction, not an asserted original symbol. The symbol table
and initializer reference are updated together, with the actual callback
prototype.

The match adds one function and **0.264350 percentage points** of exact code
against this checkout's code denominator. Current totals belong in
[PROGRESS.md](../PROGRESS.md). This work started at `3f376f6`, before the user's
parallel integration; its aggregate is not the user's newer upstream total.
The earlier Vorn and Golem matches are not counted again.

## How the match was obtained

The guarded draft was a starting hypothesis. Its pinned turn-selection helper
could overwrite the outer actor pointer: one directed fallback case passed
`0xDC` to navigation where the ROM passed `0x4000005C`. That draft was not active
in the production ROM. Reconstructing typed records and removing its two pins
resolved the defect and supplied a useful compiler structure.

The preceding Golem reconstruction transferred most of the actor, group,
navigation, turn-selection, terrain, and state/control operations. Two
target-specific decisions mattered before stack ordering could be isolated:

* Each outer flag query and clear follows the ROM's actual read/write boundary.
* Navigation failure and terrain failure have separate clear-and-return paths
  in this function's route helper. The larger Golem function uses a different
  shared tail.

Evaluating a word-clear mask before its load removed the last instruction-order
difference. At that point the full extent, frame, opcodes, physical registers,
branches, literals, and data agreed. All **131 differing bytes** were offsets in
SP-relative accesses to 45 generated address slots. Many materially different
source hypotheses had the same 131-byte distance.

For the observed `(plus:SI (reg:SI R) (const_int O))` expressions, this compiler
orders PRE entries by:

```text
bucket = (13772 + R + O) mod T
T = max(11, N // 2) | 1
```

`N` counts real instruction, jump, and call entries after address-of elimination,
immediately before GCSE. Equal buckets retain first-discovery order. These
rules were verified against actual candidate allocations before using them to
infer the ROM's required ordering. They are not a universal formula for all
stack slots or expression shapes.

A conditional composition model evaluated measured source alternatives
algebraically. Its 3,359,232 combinations were inexpensive ordering calculations,
not that many source compilations. Of the feasible predictions, two used the
minimum three changes. The first selected composition compiled to an exact
match:

1. The cell query accepts a position and owns its coordinate and bounds checks.
2. The state-selection implementation accepts the stored byte domains for
   state, phase, and behavior.
3. Terrain height reads each coordinate's fractional byte directly, removing
   the extra fraction getter.

| Measured quantity | Stack-only predecessor | Exact composition |
| --- | ---: | ---: |
| Real input instructions, `N` | 2,573 | 2,534 |
| PRE table size, `T` | 1,287 | 1,267 |
| First turn actor pseudo | 807 | 783 |
| Arrival-to-turn pseudo distance | 691 | 667 |
| Generated address slots | 45 | 45 |
| Differing bytes | 131 | 0 |

The combined compile verified the predicted interaction. Source transformations
are not assumed to compose additively in general. The independent reduced
ordering inequalities were checked against direct sorting on 45,000
deterministic assignments, including collision boundaries. The ordinary byte
comparison remains the acceptance gate.

## Cleanup and what the source does not prove

The review of the earlier report was fair. Matching bytes establish the output
of this C under the target toolchain. They do not establish historical helper
names, a unique original type graph, or the original authors' abstraction
boundaries. Receiver types, macros, and inline boundaries were deliberately
selected in response to compiler evidence. Describing every such choice as
recovered original design was too strong.

The new function now uses the existing `SceneActorRegistryResetView`. Removing
its entire duplicate actor declaration preserved every byte immediately.
Eleven unused transplanted functions/macros were removed. The retained body
record contains only the observed signed pursuit timer, and the terrain
override's names and offsets are corrected. Script and animation calls use the
existing typed declarations and explicit receiver-view casts.

The earlier 8,764-byte `Golem_UpdateHitReaction` (`080C9C04–080CBE40`) can also
use the shared actor declaration. A first nested overlay changed address
formation at `+0x310`, `+0x318`, and `+0xE9`. A focused repair recovers the two
position records directly in the shared header and uses the existing direct
heading member. The exact target hash remains
`e84861a56dd140dc051d61b2438dec3e0b254bcfd6a89e98277eb03514b5f4df`.
The old unknown byte range had no member-access users in this checkout. Its
replacement keeps all later offsets and the complete struct size unchanged.
That cleanup is a separate commit from the new match.

The `defaultFlags2CC` and `defaultFlags2D0` names already existed in the shared
view: they describe the live fields whose defaults are stored, not the defaults'
own offsets. A header comment now makes their actual `+0x118/+0x11C` locations
explicit.

Some matching choices remain: layered flag getters, the one-pointer random-table
view, and typed/generic helper interfaces. In the larger function the response
control/state wrappers still duplicate other interfaces. Their particular
definitions are not proven necessary across all possible matching source.
Across the two reconstructions, 41 of 46 same-named inline helper definitions
are identical after prefix and whitespace normalization. A shared internal
header is a concrete next cleanup experiment, with both complete byte checks
kept as gates. This batch does not claim phase 4 is finished.

## Verification and retained evidence

The cleaned source passed the ordinary evaluator, a separate uninstrumented
stock compiler, and a complete translation-unit production compile. Each
produced the exact ROM interval, with zero compiler warnings for this target.
Assembler stderr was empty under the production `--no-warn` command. The
same-byte trace independently verifies all 45 address-slot roles.

All **117 directed ROM/candidate cases** agree, visiting 360 distinct target
basic-block starts. They compare declared call arguments and ordering, full
navigation coordinates, nonstack writes, read addresses, and returns. The
fixtures cover all three arrival expansions, selected-player variants, random
table boundaries, terrain overrides/slopes, route outcomes, and call side
effects. Their frozen ABI contracts and stubs are explicit; this is bounded
supplementary evidence, not a whole-game execution proof. The target compiler
also passed **183 layout assertions across 15 records**.

After the rename and shared-header cleanup, both final unit checks pass. The
combined build printed `build/boktai3.gba: OK`; the RAM-literal check passed.
All 16,777,216 rebuilt ROM bytes equal the supplied reference, with SHA-1
`2651c5e6875ac60abff734510d152166d211c87c`. The final ELF contains the new
6,372-byte Skeleton symbol and the unchanged 8,764-byte Golem and 660-byte
Vorn symbols at their original addresses.

The shared-header gate independently verifies 201 unchanged member offsets and
sizes, total size `0x6EC`, alignment four, and all seven Golem neighbors in both
guarded and draft-enabled builds. Its three existing pointer warnings in
`Golem_UpdateDefeat` are unchanged; the cleanup adds none. Final checks and
delivered commit identities are recorded in the package's `validation.json`
and `README.md`.

The three contact search histories retain 76 distinct context-bound node IDs
across 26 root, 32 structure, and 30 independent-compiler records; shared states
appear in more than one history. Cleanup experiments have separate histories.
This is a second large-function success, not a throughput measurement for the
remaining hundreds of drafts.

The deliverable preserves the final source overlay, separate patches, portable
source histories, the read-only slot-order diagnostic, conditional composition
model, directed fixtures, and compact validation reports. ROM data, generated
assembly, objects, executables, and active database files are excluded.

The reported cross-platform `matchsearch land` test failures were not part of
this task. This checkout's final landing succeeded and passed its ordinary unit
check; that does not resolve or supersede the user's other checkout's results.

See [the PRE ordering explanation](../tools/scope/PRE_STACK_ORDER.md) for the
conditions under which this diagnostic applies.
