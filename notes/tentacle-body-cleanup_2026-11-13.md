# Tentacle body unit cleanup

Tags: TentacleBody, 080A5B8C, 080A60E0, 080A62DC, 080A6F50, phase4

Consolidated 16 files from 080A5B8C through 080A7188 into `src/tentacle_body.c`.
The final formatted unit matches all 5628 bytes with the stock compiler.

The header describes the complete 0x934 body allocation, verified against
TentacleEnemy_Create: four existing TentacleAnimationSegment records, spin,
attack flags, segment count, release frame, and spread stage. Existing actor,
collider, sprite, tile, sequence, profile entry, and saved-state types replace
private padded views and raw field offsets. The common actor prefix remains
flat because the canonical reset view continues past this species allocation.
No shared engine header changed.

Four anonymous callbacks receive provisional TentacleBody names with low
confidence and interpretation notes. Parameters and segment-loop locals now
name their roles. Existing meaningful names remain, including the shared-style
ActorTrail_RestoreSavedState entry. No external source references needed edits.

All four animation-selection callers now use one ordinary inline helper.
The direct actor-field version from attack recovery matches all four callers;
the earlier cursor-based helper added 16 bytes to recovery. Common spread flag,
shift, and status helpers replace repeated copies, and charge spread uses the
shared control transition. The saved-state readiness accessor must remain:
replacing it with a direct field test changes 55 instructions, 19 beyond register
allocation. Restoring the accessor matches with the shared entry-flag helper.
The existing multi-use spread-offset macro remains unchanged.

Validation: all 16 typed inputs MATCH; a second helper/declaration pass exposed
the two differences above, both resolved with focused checks. Final merged unit
MATCH before and after formatting/local naming. No new concessions or ROM build.

The weekly run ended during this item. It was completed; Skeleton and subsequent
queue entries were not started. The coordinator should run combined validation.
