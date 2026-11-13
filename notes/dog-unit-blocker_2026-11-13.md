# Dog consolidation blocker

Tags: Dog, 080DDD28, 080E0750, 080E25FC, 080E4B34, phase4

Deferred the full Dog range 080DDD28–080E6E08 before type consolidation. The
queue describes it as concession-free, but three source files select different
shared chase implementations with SCENE_ACTOR_CHASE_* definitions. A single
translation unit would use only the first included configuration. Retaining
those selector macros is prohibited by this worker's brief.

Removing selectors with all other C unchanged produced:

- 080E0750: 95.5% fuzzy, 152 register-only instruction differences, 4232 bytes.
- 080E25FC: 96.0% fuzzy, 87 register-only instruction differences, 4500 bytes.
- 080E4B34: 96.7% fuzzy, 32 register-only instruction differences, 1976 bytes.

The first two request a wide return from SceneActorChase_MapWidth, consumed by
CellCenter inside Arrive. The third selects direct saved-control reads and a
scalar CellIndex layer, also reached through Arrive. These are shared inline
chains, not three direct call-site substitutions. Duplicating the route helper
for each configuration would undermine the requested source cleanup. Changing
the shared default requires checking all consumers and coordinating with the
phase-3 workers; it is deferred rather than hidden in a unit-specific copy.

All three original sources restored and rechecked MATCH. No production source
or header changes landed for Dog. The next attempt should resolve those shared
helper variants first, then use the recorded per-function list and existing
context field evidence for the 73-file consolidation. This is a phase-4
consolidation deferral, not a claim that ordinary C cannot match these routines.
