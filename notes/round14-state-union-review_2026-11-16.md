# Round 14: shared state operations and ordinary union alternatives

Tags: #round14 #matching #concessions #object-model #cleanup

Date: 2026-11-16. These are fixed source hypotheses, not a spelling or declaration
order search. Fourteen complete candidates cover seven units: one matches,
twelve compile but differ, and one exposes a conflicting shared declaration.
Only the exact candidate is installed.

## Accepted control transition

`SceneActor_BeginReactionAnimation` in `src/monsters/octopus.c` now calls the existing
`SceneActorChase_SetControl(actor, Ivy_UpdateActiveState_080E98B4, 0x22)` when the
actor restarts. The repeated stores already describe precisely this shared
operation: prepare the step, reset completion, select the control kind and
callback, and clear pending-control state. The operation is already used by
related actors and its owning header was already included.

Integration commit `679289315` replaces sixteen lines of duplicated state setup
with that call. It removes one reported constant local without adding a helper,
alias, cast, wrapper or header dependency. All **14,980 bytes** of the complete
Octopus unit pass an independent stock check; its compiler-aware count falls from
15 constant-local warnings to 14, with no new warning kind or site. The separate
initial-state zero in this function remains visible.

## Open shared-operation candidates

The same hypothesis was tested for Octopus's `Ivy_ResetPhaseActive`,
`ResetState_ReturnToIdle` and `SetState_BeginReturnToRoutePoint`, and Ghost's
`Ghost_SetIdleBehavior` and `SceneActor_BeginLinkedIdleReaction`. Replacing their
duplicate operations with the corresponding existing state, behavior or control
helper does not preserve the full-unit match. These five drafts remain open;
the experiment does not establish that the helpers or the existing drafts are
the original structure.

For Lun's `SceneActor_BeginLinkedIdleReaction_0809C834`, including the shared
chase-reaction header exposes a real declaration disagreement:
`gUnknown_02000580` is a `struct Player *` in Lun and a pointer array in the
chase-shared header. No typedef alias or incompatible cast was introduced to
conceal that disagreement. A separate candidate uses Lun's already available
local state/control operations and compiles, but does not match. Resolving the
shared player-slot declaration belongs to a coherent type cleanup with every
dependent checked.

## Open union and field candidates

| Function | Ordinary source tested | Result |
| --- | --- | --- |
| `RisingActor_AdvanceHeight` (`081FDAF4`) | Replace the halfword-built zero with a literal; then use the actual volley and collider fields | Both differ; the 76-byte target retains a saved zero and mask materialization |
| `DriftingSprite_Update` (`08126170`) | Replace the one-member flag union with the actual sprite flag field | Compiles, but does not match the complete 444-byte unit |
| `Route_Start` (`0821E5C4`) | Keep the definition and point-array values separately instead of sharing a union | 80 bytes against the 84-byte target; the extra copy/save disappears |
| `ContactActorPool_Initialize` (`08043C4C`) | Separate the actual pool and slot pointers; then index the real slot array | Both differ from the complete 148-byte target |

No new union, reused unrelated local, dummy operation, suppression or compiler
setting was adopted. These six failures measure specific natural alternatives;
they do not justify the existing storage unions or exhaust the source choices.
The linter also reports some union definitions outside function bodies, so its
`local-union` total is a review queue rather than a count of proven artificial
overlays.

## Replayable evidence

[round14-state-union-candidates_2026-11-16.jsonl](evidence/round14-state-union-candidates_2026-11-16.jsonl)
contains all fourteen complete source patches, their canonical source-blob bases,
hypotheses, whole-unit extents, outcomes, and SHA-256 hashes. All fourteen patches
were independently applied to their named baseline blobs and verified against
the candidate hashes. The file contains no generated assembly or ROM data.

Each record is an independent source snapshot. Reconstruct it from its own base
and review it before checking it against the current headers; do not apply the
catalogue as a patch series. The original temporary check results remain under
`build/round14/state-recovery-pass/` and `build/round14/union-fields-pass/`.
