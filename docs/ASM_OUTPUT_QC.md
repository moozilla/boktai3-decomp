# Empty-asm output and incidental-return QC

Reviewed 2026-10-15 against full source, ROM assembly, and direct callers.
An allocation-only token used solely by empty asm is different from a scalar
returned to C callers. The former carries no game-state value; the latter
requires a genuine return-value contract.

| Entry | Reviewed bytes | Output usage | Result |
|---|---:|---|---|
| 08059C48 ScenePanelRig_TickHoverSelection | 432 | R1 token consumed only by empty asm around selected-subject store | Keep register-pinning concession; deferred clobber cleanup |
| 08054EC0 DisplayTrack_Update | 936 | R2 token consumed only by empty asm around scale comparison | Keep concession; deferred clobber cleanup |
| 08056298 TerrainEdgeMarker_FindTerrainDirection | 1448 | R3 token consumed only by empty asm across signed coordinate load | Keep concession; deferred clobber cleanup |
| 080674F0 SceneActorControl_TryActivateNearby | 360 | R3 token consumed only by another empty asm | Keep concession; deferred clobber cleanup |
| 08055EEC StageIconDisplay_Initialize | 80 | Old output-only scalar escaped through return | Replace with actual selector call result; exact ordinary C |
| 08055FA4 StageIconDisplay_InitFromScript | 124 | Old output-only scalar escaped through return | Replace with actual selector call result; exact ordinary C |
| 08054B64 HitSwitch_AddFromScript | 28 | Old output-only scalar escaped through return | Restore ASM; explicit-zero draft remains nonmatching |

## Pure allocation tokens

The four retained tokens have unsigned word type. They feed no C arithmetic,
branch, address, store, or return. An output-only constraint defines a compiler
register token, and the later empty asm is its only consumer. These are marked
register-pinning concessions, not recovered data values. Do not infer that their
bits equal a nearby source variable merely because a register happens to hold it.

Replacing each whole token lifetime with explicit clobbers, or deleting the
whole token, produced complete same-size near matches but changed bytes:
59C48 changed21,54EC0 changed2,56298 changed6,674F0 changed3. Cleanup is deferred;
no inexact replacement was promoted. Never let depin delete only a defining
output while retaining a later read of its C variable. If both ends become
removable without byte changes, remove the complete token lifetime.

## Stage icon result propagation

The actual 55EB8 selector stores an update callback and ends `pop {r0}; bx r0`.
Its word result is therefore the saved Thumb return address. The word has no
established game-data meaning; both observed creators test only its sign:
55F3C calls55EEC, and56020 calls55FA4. Both create/kill decisions require a
nonnegative result for success. The two initializers forward the selector's
returned word through ordinary C; they no longer invent a value with empty asm.

The selector declaration is now `s32 StageIconDisplay_SelectUpdate(...)`.
The existing void C definition cannot provide that actual result contract and
must move behind an ASM default boundary before the combined build. Its full
52-byte extent is excluded from exact C progress. The valid draft returns0,
preserving the two creators' observed success sign while deliberately differing
from the ROM's incidental return-address bits. This draft must remain guarded
and must not count as matching. Attempts using builtin return-address access
also differed from the ROM and were not promoted.

The complete80/124-byte initializer extents remain exact after replacing their
opaque result with the actual call result and removing concession markers.
Source search and the reviewed ROM have only these two direct selector callers;
no other source caller requires a return-usage update.

## Hit-switch wrapper

54B64 calls54834 and forwards its incidental R0. The void668-byte initializer's
common exit likewise pops the saved return address into R0. Giving its C wrapper
a value with output-only empty asm would let an opaque token escape into C.
Retaining the wrapper's28-byte ASM body is the smaller correction than replacing
the668-byte initializer. The exported valid C draft calls the initializer and
returns0; no unrestricted equivalence for the original returned bits is claimed.

The two ASM fallback extents total80 bytes (52+28), approximately0.0033 percentage
points. The204 initializer bytes stay exact and move from register-pinned to
ordinary C. All source-body fixes have individual full-extent fast checks; the
integrator must guard52/28, record nonmatching scores, and run the combined ROM
build before merging.

## Consumed fallthrough follow-up

A second read-only pass reviewed48248,506B8,513B8,5173C,51B70,56DBC,5C8D8,
612D0,61CA8,6374C and66DB8 against their direct callers and ROM exits.

506B8 and5173C are task destructors;56DBC and5C8D8 are task updates. Their results
are discarded by the observed callback driver. 6374C is called only by6385C,
which immediately sets its own result to0. 61CA8's sole direct caller61CFC
ignores it and immediately loads1. These warnings do not demonstrate consumption
of a missing return value. 612D0's result is consumed by61328, but only categories
outside the documented0/1/2 API domain reach its missing-return path; no new
out-of-domain caller was established. These are narrower findings than a claim
that every nominally nonvoid callback has meaningful return bits.

### Directional force513B8

Creator51574 tests this initializer's result for negativity. Its former
fallthrough was therefore a real C issue. The repaired full444-byte exact body
tracks a defined status word from theH keyword lookup/value, then, for modes1/2,
from the range helper's lastt-keyword lookup/value and each executed Mod call.
Arithmetic stored into magnitude/delay uses copies of those results; it does
not replace the tracked status. This models the ROM's retainedR0 on every mode,
including0 and unsupported modes, rather than inventing a constant success.
The final formatted source is exact, with8 minimized register hints.

### Contact burst48248 and collider refresh821FE40

Creator48344 tests the initializer's success result. The final Collider_Refresh
ROM body ends `pop {r0}; bx r0`, returning its saved Thumb return address rather
than the void C definition's missing result. Its canonical declaration is now
s32. The initializer explicitly forwards that call result and returns-1 on
resource-load failure, through a common return. Its full252-byte body is exact
ordinary C after depin1→0. The integrator must retain the complete44-byte
Collider_Refresh ASM boundary; the exported C draft normalizes its incidental
positive word to0 and remains nonmatching. Other local declarations are
normalized by the integrator before its combined build.

### Tether-nearby predicate51B70

The result is consumed by51BD8 and51EA8. In the inside-box but outside-circle
corner, the ROM does not overwriteR0: it returns the nonnull tether-position
pointer as a word. Returning0 would change this observed nonzero quirk. The
integrator retains the104-byte ASM boundary with a valid pointer-return C draft;
the attempted defined C return did not reproduce the exact allocation.

### Attachment descriptor updater66DB8

This pointer result is stored by many callers. Unsupported kind127 with frame0
is explicitly reached by49258 and4AB40, so a blanket kind0..5 precondition would
be false. The ROM's ordinary update path returns the result of its final
ReleasePartsResource call when the kind is unsupported. With frame127, an
unsupported kind instead preserves the original input slot. Null descriptor or
null slot in the release-only path returns0. Known kinds0..5 retain the existing
constructor/release results.

The complete defined candidate explicitly returns those values on every path.
It accepts word arguments and narrows kind/frame to8 bits and count to16 bits,
as the ROM does. It remains472/472 bytes with12 different bytes and95.8715596%
fuzzy score. The difference is allocation and load order, including the second
jump table's cursor/base registers. No opaque output is returned to C. Freeze the
whole472-byte ASM boundary and keep the candidate guarded; do not count it as
exact. Export: `wip/handoff-drafts/sub_08066DB8.c`; the integrator records its
nonmatching score on the main-target batch.

The additional conservative ASM boundaries in this follow-up total620 bytes
(44+104+472); the earlier80-byte correction remains separate. Both repaired
initializers513B8/48248 retain their complete exact extents. Worker checks were
per function only; combined ROM validation belongs to the integrator.
