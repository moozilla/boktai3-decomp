# Round 14 checkpoint 4: Feather callback interfaces

Tags: phase3, checkpoint4, callbacks, feather, shared-types, interface-audit

Source base: `720eb4fadaebcf3d8adb8c9303a680ea6e99aafb`.
Integrated correction: `a97420c5fb2487cdc3fcf36e9515f2accee81ef1`.

The earlier pool-cursor change gave the four-entry Feather state table a direct
`void(FeatherPool *, Feather *)` callback type, but its definitions still used
incompatible signatures and private record layouts. This follow-up corrects
that interface: all four definitions include the owning header and use its
actual callback parameter types. The shared Feather record now embeds the
existing sprite component, and the header declares each target explicitly.

**All four direct table targets and the updater compile without diagnostics.**
All six owner-header dependents match their complete original units, totaling
1,092 bytes. The unchanged creator caller also matches its complete 540-byte
unit: seven units and 1,632 bytes checked in all.

This is an interface correction. It does not remove Scatter's existing helper
debt, canonicalize the entire private Feather family, or repair the scheduler's
generic callback contract. Those boundaries are recorded below.

## Table and shared component

The four entries at `gUnk_086107A0`, corroborated by `gen/data.s`, are:

| State | Actual target | Canonical definition | Complete unit |
|---:|---|---|---:|
| 0 | `sub_08128B48` | Empty callback taking `FeatherPool *` and `Feather *` | 4 bytes |
| 1 | `FeatherEffectPool_UpdateOpeningAnimation` | Same pool/slot callback type | 392 bytes |
| 2 | `FeatherEffectPool_UpdateScatterAnimation` | Same pool/slot callback type | 368 bytes |
| 3 | `FadingSprite_UpdateExpiry` | Same pool/slot callback type | 112 bytes |

The former state-zero source described a module-initialization hook and used
`void(void)`. Its table membership establishes the two ignored state-callback
arguments. No symbol or ROM table entry was renamed.

`include/effects/feather_effect_pool/feather_effect_pool.h` now declares all four prototypes and the exact
four-entry table extent. A stock-compiler probe initializes a real
`FeatherStateCallback[4]` from those four names in ROM order; it produces no
diagnostics. Each definition includes the same header, so an incompatible
definition is also checked within its own translation unit.

`struct Feather` embeds `struct ContactSwarmSprite sprite` as its first member.
This recovers an existing component; it does not overlay a new view or insert
a wrapper. The callback bodies use `sprite.flags` and pass the native sprite
and `DisplaySequenceState` objects to playback. Opening's two prior component
casts disappear. Expiry's private `active` and `resetBlink` bytes become the
shared `state` and `enter` fields at the same offsets.

Two probes compiled by pinned ARM agbcc establish the preserved layout:

| Property | Measured value |
|---|---:|
| Sprite component size | `0x2C` |
| Feather sprite offset | `0` |
| Sprite flags / priority / frame / position | `0` / `7` / `0x10` / `0x1C` |
| Feather sequence | `+0x2C` |
| Feather age / state / slot | `+0x90` / `+0x92` / `+0x93` |
| Feather blink age / enter flag | `+0x94` / `+0x96` |
| Feather size | `0xA0` |
| Task header size | `0x18` |
| Pool occupancy / first slot | `+0x38` / `+0x3C` |
| Slot count / complete pool size | `20` / `0xCBC` |
| Callback count / pointer size | `4` / `4` |

The existing `ContactSwarmSprite` header was not changed. Its opaque bytes still
cover the graphics-pointer position that some private Feather views name;
recovering further sprite fields is not part of this callback correction.

## Scatter helper boundary

The first natural candidate called `PlayerCue_StepSequenceEnded` directly and
removed Scatter's local `Step` helper. It compiled cleanly but produced 364
bytes rather than 368. The changed sprite/base lifetimes remove an initial
copy and alter register assignments; alignment also changes. The complete-unit
comparison reports 31 differing instructions, or two with registers ignored.

The accepted interface repair retains the **existing** helper shape:
`Step(owner, state, base)`. Its owner is now a real `ContactSwarmSprite *`, its
state parameter a real `DisplaySequenceState *`, and its base the shared
`Feather *`. As before, the helper uses `base->sequence`; the state parameter
is unused, and owner/base refer to components of the same feather.

That duplicate-argument shape remains source debt. It was not introduced by
this repair, moved to another wrapper, or credited as cleanup. Correcting only
the actual component types restores the exact 368-byte unit and removes both
old incompatible playback-argument warnings. The failed direct-call candidate
is preserved as a complete replayable source patch.

## Creator and scheduler boundary

`src/effects/feather_effect_pool/sub_081297DC.c` now includes the Feather owner and removes its false
`void FeatherEffectPool_UpdateTask(void)` declaration. The creator returns
`struct FeatherPool *`, using its existing allocation local as an opaque
object pointer through the pre-existing task APIs. This removes the old
incompatible return warning without a cast or a new helper.

The sole existing caller, `FeatherEffectPool_CreateDirectional` in
`src/effects/feather_effect_pool/sub_081294B4.c`, already declares a `FeatherPool *` return. Its complete
540-byte unit was checked. **Only the return tag/address meaning is aligned:**
that caller still defines private Feather and truncated FeatherPool records.
This is not a claim that the creator/caller family has fully canonical types.

Exposing the true updater declaration produces one compiler warning that the
false declaration previously concealed:

> passing arg 2 of `Task_SetCallbacks' from incompatible pointer type

The creator's unchanged private registration prototype accepts `void(void)`
callbacks. The implementation in `src/engine/task_scheduler/task_scheduler.c` instead takes callbacks
on `RuntimeTask *`; `include/engine/shared/engine.h` has a generic `void *` callback contract,
and the shared `Task` header uses its own task-pointer type. The Feather updater
actually returns `u32` and takes `FeatherPool *`. The creator also retains its
pre-existing false `void(void)` destructor declaration, whose implementation
uses a private pool record and returns `u32`.

These are existing scheduler/type-erasure issues, separate from the four state
table calls corrected here. No alternate registration signature, function
pointer cast, suppression, wrapper, or task-layout redesign was added. The
warning remains visible in the captured stock compiler diagnostics.

## Compiler diagnostics and scope

The normal unit checker discards successful compiler stderr, so this review
also captured stock agbcc diagnostics directly for every checked source.
The results distinguish compiler type warnings from the concession linter:

| Source | Before | After |
|---|---|---|
| Opening, empty state, updater | No compiler warnings | No compiler warnings |
| Scatter | Two incompatible playback-component arguments | No compiler warnings |
| Expiry | Incompatible private pool-pointer initialization | No compiler warnings |
| Creator | Incompatible pointer return | Exposed scheduler callback incompatibility |
| Existing creator caller | No compiler warnings | No compiler warnings |

The owner header now has six dependent source units: `sub_08128B48.c`,
`sub_08128B9C.c`, `sub_081292D4.c`, `sub_08129444.c`, `sub_081296D0.c`, and
`sub_081297DC.c`. Every one was independently checked against the ROM with an
explicit complete-unit extent. The updater itself required no source edit.
The extra caller check accounts for the seventh unit.

Remaining private family records are deliberately outside this bounded fix:

| Existing source | Remaining private view |
|---|---|
| `src/effects/feather_effect_pool/sub_08128B4C.c` | Allocator's private `S`/`E` pool and slot records |
| `src/effects/feather_effect_pool/sub_08128D24.c` | Clip-path Feather view |
| `src/effects/feather_effect_pool/sub_08128F98.c` | Ground-scatter Feather view |
| `src/effects/feather_effect_pool/sub_081294B4.c` | Directional creator's Feather, pool, and vector views |
| `src/effects/feather_effect_pool/sub_0812975C.c` | Destructor's private pool/entry records |
| `src/effects/feather_effect_pool/sub_081297A4.c` | Initializer's private pool prefix |

The local types named Feather in `src/effects/large_fragment_effect/large_fragment_effect.c` are a different
record: their state/slot/enter offsets differ. Name similarity alone is not
sufficient to migrate them into this state-table family.

## Commits and durable evidence

| Concern | Worker commit | Integrated commit |
|---|---|---|
| Owner declarations and embedded sprite | `5f56e235a7` | `567ac15a9` |
| Empty state signature | `2cd6bfa8a3` | `31f447575` |
| Opening callback | `37ba1d4f25` | `8af963167` |
| Scatter callback | `2674508751` | `93511eb85` |
| Expiry callback | `93ddbf8238` | `d1c16754f` |
| Creator declaration and return tag | `4458ea8774` | `a97420c5f` |

[The evidence record](evidence/round14-feather-callback-interfaces_2026-11-16.json)
contains the source hashes, complete-unit results, before/after compiler
diagnostics, header dependents, both ARM layout probes, the typed table probe,
and the rejected direct Scatter candidate. Its nonempty patch is against the
integrated source revision above and was applied in a temporary tree with the
resulting source verified. All seven source files and the owner header were
also compared between the worker and integrated source revisions.

Validation used stock agbcc revision
`da598c1d918402c42c0c0d7128ba14567f3175e9` and the reference ROM with SHA-1
`2651c5e6875ac60abff734510d152166d211c87c`. No compiler/linter edit or generated
assembly/RTL is included. The coordinator owns the final combined header
checks, census, and full ROM build.
