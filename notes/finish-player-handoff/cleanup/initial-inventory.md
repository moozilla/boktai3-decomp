# Player subsection cleanup inventory

Tags: #player #structs #data #studio #register-pinning

Read-only inventory after matching0814D264. No cleanup source/header/data/Studio mutations. This snapshot uses current symbol names and source, with checked-in PROGRESS.json only for unit membership; that JSON's matching states are intentionally stale until batch integration.

Scope: Player, Player burst, Player attack effect and Player clone report units. The containing Player category also has cue/contact/registry/orbit/barrier/cloud units; their current ownership should remain explicit in a full subsection cleanup.

## Existing shared ownership

- `include/player.h` is the canonical Player and PlayerStatModifierContext owner. It already holds action request/config/entry/drain records, collider and cue-collider views, animation/visual object records, and gPlayers. It is substantially more informative than most local padded Player views.
- `include/player_3.h` owns modifier callback input/target records only; these are distinct callback arguments, not additional Player prefixes. `notes/player_3.md` records the prior unit merge and field choices, although its matching-state narrative is historical.
- `include/contact_collider.h` owns the generic0x54 contact collider; `include/scene_vec.h` owns signed padded8-byte SceneVec. `include/display_sequence.h`, `display_keyframe.h`, `display_animation.h` and `contact_swarm.h` provide existing graphics/sequence records used by the attack particle allocator.
- `include/monster_particle_random.h` already provides shared random cursor/sample operations. It should be reviewed as prior art before adding a Player-specific random-helper family, while preserving the successful matching form's source/lifetime boundaries.

## Strong struct/header consolidation families

1. **Player attack context.** Fresh D264 uses AttackMotionContext over PlayerStatModifierContext, with embedded collider at+48, dest at+9C, origin at+A4, speed/phase/timer/heading at+BC/+C0/+C2/+C6/+C8. `SweepCtx` in0823DAB4, `SpinCtx` in0823E250 and `ThrowCtx` in0823E998 describe the same attack storage. Canonical PlayerStatModifierContext already names much of the tail, but represents the collider through the smaller drain view and leaves dest opaque. A reviewed shared attack-context view/union is the natural family consolidation. D264's separate Player fields at9C and C1A should become canonical Player members instead of another padded owner view.
2. **Attack cue descriptors.** `PlayerActionCueInfo` is copied locally in0814D7F8,0814D92C and0814D9C4. Shared declaration and typed owner fields are a narrow initial migration.
3. **Action emission arguments.** Identical `BurstA`/`BurstB` records recur in0814E3D8,0814E990,0814F328 and0823F148. Consolidating the records before changing their inlined operations can separate layout validation from allocation changes.
4. **Player burst/transition task.** `PlayerTransition` repeats across0815FEB0,0815FFDC,08160060;081602D0 calls the same records PlayerStatEffect. Task_Create requests0x4C8 in0816033C. The particle initializer's task view has owner at18, sheet atF8 and sixteen records atFC. Its BurstPosition has an explicit4-byte alignment and unsigned components; preserve those properties rather than automatically replacing it with signed SceneVec. FrameDisplay in08160380/0816044C is another small shared record family inside this report unit, but its separate identity should remain descriptive.
5. **Player clone pool.** Four local PlayerClone layouts exist.082003BC and08200C94 are the strongest whole-pool layouts: three entries, stride344, task header/active mask, count at9E8 and two palette banks. CloneParams/CloneCollider/CloneAnim/CloneState/PlayerClonePool repeat.08200288/08200324 are smaller views;08200A10 and08201160 still use raw slot offsets. Reconcile the two strong layouts first.082003BC deliberately uses signed16-bit bitfields for flags, whereas08200C94 uses plain unsigned fields; this is a compiler-sensitive access view, not evidence of different storage. Keep signedness/memory modes explicit during migration.
6. **Global singleton types.** `gPlayerAttackEffect` and `gDirectionalSpritePool` are still u8* in ram.h and symbols/ram.csv. Give each a forward-declared canonical task/pool pointer only after the owner records are reconciled. The global's name-to-address relationship is already centralized; avoid new aliases such as gPlayerCloneManager.

The scratch file list covers476 source files in these four report units. Repeated local struct names and their paths are in `player-cleanup-structs.json`; complete file membership is `player-cleanup-files.txt`. Generic names PackedVec/Vec/P/Obj/S cover unrelated owners and should not be merged by name alone.

## Data and Studio

No Player-specific decoded tables are currently in data/manifest.csv. Existing editable command arrays live under data/actor_commands; Studio only selects callback_array manifests whose source starts with that path. The framework supports exact ordered Thumb callback CSVs already, including null entries and relocatable pointers.

Seven Player callback tables are ready for a narrow first data batch after naming/extent review. Read-only decode plus callback_array.encode validated every current word, index and extent:

| Range (end exclusive) | Entries | Current reader/use |
|---|---:|---|
|08610964–08610980|7|0813FC38 selects Player update callback by actionMode|
|08610B70–08610C0C|39|0815788C tether-mode callback dispatch|
|08610C0C–08610CA8|39|08157CFC alternate tether-mode callback dispatch|
|08610CA8–08610D18|28|0815849C mode dispatch|
|08610D18–08610D88|28|08158754 mode dispatch|
|08610D88–08610E24|39|08158848 tether-mode callback dispatch|
|08610E2C–08610E44|6|0816233C attack emission selection by element|

Numeric tables need a fixed-width numeric-record format or another reviewed editable representation:

-086108F8–08610918: eight4-byte directional-effect records used by0813BA88 (frame, horizontal/vertical flips, unknown byte).
-08610918–0861091C: four effect-tick threshold bytes used in player_3.c.
-0861091C–08610940 and08610940–08610964: eighteen u16 mode parameters each, selected by engine flag1000 in ActorParameter_GetModeValue.
-08610980,086109F4,08610A68,08610ADC: four0x74-byte u16 lookup tables, selected for kinds0/1/2/6 by ActorVariant_GetTableValue081419D4.
-08610B50–08610B70: sixteen signed16-bit input-key direction values used by Player_GetInputDirection08156BBC.
-08610E24–08610E2C is an eight-byte gap/table between callback families; inspect its consumers before assigning a format.

Studio's existing Code tree already groups these units through script_targets.progress and current function symbols. /api/actors includes command_tables; atlas.command_table_blocks exposes their decoded extents in the ROM tree. For Player tables, generalize that ownership/path filter and show the dispatcher/index meaning, with direct links from Player code/actor pages. Reuse current callback-table validation and symbols rather than making a separate hard-coded Player table list.

## Phase-three priority based on read-only removal diagnostics

`player-cleanup-pins.tsv` is the current source-derived inventory:73 functions in Player,2 in Player attack effect and2 in Player clone; none in Player burst. This includes functions reached through hinted helpers. These are an audit count, not a progress-report update.

Best narrow starts after remaining matches land:

| Target | Current concession | Removal-only result | Why it is a useful small next task |
|---|---|---|---|
|08156BBC Player_GetInputDirection|one keys liveness operand|2ROM/2draft|96-byte function; numeric direction table is also a clean data owner|
|08201160 clone creator|one pool liveness operand|1ROM/1draft|272-byte function; likely C copy/init lifetime relation, with typed-pool migration as useful structure|
|0814DF0C Player_IsStatBBelow|one r3 clobber|2ROM/1draft|76-byte branch helper; shared override fields already canonical|
|0813B18C relative cardinal direction|one memory barrier|4ROM/0draft plus moved branch|96-byte shape/threading issue, rather than a broad register-allocation search|
|0814A854 ActorFlags_GetResponseBits|one loop-index pin|7ROM/7draft|48-byte callback loop; can migrate flagCallbacks from canonical Player simultaneously|
|08141A1C and0823C368 history selectors|same two pins each|first twin all removed13ROM/13draft|Byte-for-byte source twins; solve one ordinary helper and test both. Missing=-1 constant copy order and global sentinel lifetime are the relevant relations|
|082012D4 clone reactions|one reset-value pin|8ROM/8draft|Single reset lifetime across three slots, good companion to canonical clone pool|

No listed removal is free by itself. `scope.py FILE pins` generated trials only under build/scope/drafts; tracked source is unchanged. Dedicated trace runs are D264_inventory_keys, D264_inventory_clonecreate, D264_inventory_stat_b, D264_inventory_direction, D264_inventory_response, D264_inventory_history and D264_inventory_clone.

Larger families to keep together: paired primary/alternate action handlers; the Sweep/Spin/Throw attack-context family; the four action-emission argument consumers; and the attack allocator's inlined sequence step. The latter duplicates DisplaySequenceState operations with many internal pins, so a shared-layout migration alone will not remove its compiler constraints. Its sequence logic can still use the established display headers for ownership.

## Suggested order

After the three remaining ordinary matching candidates land: freeze exact evidence; clear the narrow barrier/copy families above; consolidate clone and burst task records with per-consumer checks; reconcile Player attack context and cue/argument records; install reviewed Player callback/numeric data and Studio ownership; then consolidate contiguous source units. Preserve each matching function's individual commit and run the combined ROM only for batch integration.
