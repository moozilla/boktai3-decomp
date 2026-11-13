# Ghost unit cleanup

Tags: Ghost, 08095574, 08096D38, 08098064, 080982E4, 080989FC, 08099B44, phase4

Consolidated 63 matching functions from 08095574 through 0809AE0C into
`src/ghost.c`. The final formatted unit matches all 22680 bytes.

`include/ghost.h` describes the complete 0x62C allocation: four 0x60-byte
afterimage records, visibility and drain counters, nine curve points, saved
movement subject and positions, and the route cursor. Sprite and animation
components reuse existing types. The actor prefix repeats the canonical reset
fields because that shared view extends beyond this species allocation.
No shared engine header changed.

Five previously anonymous functions now have provisional Ghost names. Three
misleading Bee-prefixed functions are now Ghost_UpdateCurveSway,
Ghost_ClearActionFlags, and Ghost_EndPlayerSlowdown. The word at resource
+0xC in each afterimage is tentatively called paletteWord; it is not the
sprite flags word as the previous comment claimed.

Most private availability and control helpers were replaced by the shared
SceneActor helpers with exact output. Per-function padded context views and
numeric field accesses were removed. Afterimage initialization requires two
independent record cursors; a single cursor changes 17 non-register instructions.
The timer loop retains its small goto form and a field-derived life offset:
a structured loop hoists a mask and changes allocation. The collider updater
requires a two-step position address; direct member addressing hoists it across
a branch and call. Both adaptations use the actual shared fields.

The two existing multi-use GHOST component macros remain shared in the header.
Replacing them with plain assignments changes 108 register-only instructions
in Ghost_AdvanceApproachRoute. These are existing macros used by position
addition and retreat construction, not new selector or single-use macros.
The shared ZeroFill8 helper is also unchanged; its existing volatile implementation
is outside this cleanup. No new concession was introduced.

Validation: successive complete per-function passes and focused checks for
layout/helper changes; final merged and formatted unit MATCH. No ROM build.
