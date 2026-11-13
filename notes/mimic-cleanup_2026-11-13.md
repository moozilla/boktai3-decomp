# Mimic unit cleanup

Tags: Mimic, 080E6E08, 080E7134, 080E7FF4, 080E8158, 080E87C4, phase4

Consolidated 36 files from 080E6E08 through 080E98A4 into `src/mimic.c`.
The formatted unit matches all 10908 bytes with the stock compiler.

The complete 0x664 allocation includes a navigation probe, destination, saved
origin and cell, timer, reaction flags, leap speed and health threshold. Existing
actor, player, collider, sprite, sequence and terrain types replace private
padded views and numeric field accesses. A deferred-response packet names the
saved-control snapshot, state, phase, behavior, delay and callback. No shared
engine headers changed.

Three anonymous routines are now Mimic_TryTargetRoute, Mimic_BeginTargetResponse,
and Mimic_UpdateIdleState, with low confidence and interpretation notes. The
reaction, update and cleanup wrappers explicitly pass their incoming actor.
No external rename consumers needed edits. Legacy chase declarations were
reconciled with the canonical player and engine interfaces; repeated control,
flag and predicate helpers were shared. Unused helpers were removed.

Deciding evidence: initializer sprite priority needs a local pointer to the real
field, preserving three stack-slot assignments. Direct field assignment differs
in six instruction operands. The leap's random array base must be bound at
function entry: direct array indexing in the inner block adds four bytes. The
matched form uses the named gRandomTable array and ordinary indexing. Collider
flag changes need the collider base cursor to retain the halfword displacement.

The visibility predicate now uses structured returns. Replacing target-route
exit labels with early returns changes six non-register instructions; the
original small labeled form remains. All 36 typed inputs were checked, focused
fixes rechecked, and the merged/formatted unit MATCH. No new concessions or ROM
build. Roughly 25 focused candidate checks beyond the two full input passes.
