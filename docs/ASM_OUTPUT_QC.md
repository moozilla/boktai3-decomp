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
