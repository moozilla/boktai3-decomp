# Empty-asm outputs and return-value QC

Reviewed on 2026-10-15 against complete source, ROM exits and direct callers.
An allocation token used solely by empty asm has no game-state meaning. A
scalar returned to a C caller needs a genuine return-value contract.

## Allocation-only tokens

| Function | Bytes | Token usage |
|---|---:|---|
| `08059C48 ScenePanelRig_TickHoverSelection` | 432 | R1 token around the selected-subject store |
| `08054EC0 DisplayTrack_Update` | 936 | R2 token around a scale comparison |
| `08056298 TerrainEdgeMarker_FindTerrainDirection` | 1,448 | R3 token across a signed coordinate load |
| `080674F0 SceneActorControl_TryActivateNearby` | 360 | R3 token consumed by another empty asm |

These unsigned output-only tokens feed no C arithmetic, branch, address,
store or return. Their only consumers are empty asm operands. They remain
marked register-pinning concessions; their bits must not be treated as a
recovered source value. Removing the whole token or replacing its lifetime
with clobbers changed 21, 2, 6 and 3 bytes respectively. No inexact replacement
was promoted. If both ends become removable without changing bytes, remove
both; never delete the defining output while leaving a later operand read.

## Exact C repairs

- `55EEC StageIconDisplay_Initialize` and `55FA4 StageIconDisplay_InitFromScript`
  explicitly forward the actual selector call result. Their complete 80- and
  124-byte bodies remain exact and move from register-pinned to ordinary C.
  Creators `55F3C` and `56020` test only the result's sign.
- `48248 ContactBurst_Initialize` returns -1 on resource-load failure and
  forwards the actual `Collider_Refresh` word on success. Creator `48344`
  tests that result. The complete 252-byte body is exact ordinary C.
- `513B8 DirectionalForce_InitializeFromScript` tracks actual script and random
  helper results. Mode 0 and unsupported modes retain the H-keyword result;
  modes 1/2 then retain the final t-keyword result or the last executed `Mod`
  result. Stored arithmetic uses copies without replacing the status word.
  Creator `51574` tests its sign. The complete 444-byte body remains exact
  with eight minimized register/lifetime concessions.

No opaque empty-asm output escapes into these returned C values.

## Required ASM boundaries

| Entry | Bytes | Reason and retained draft |
|---|---:|---|
| `55EB8 StageIconDisplay_SelectUpdate` | 52 | The ROM pops its saved Thumb return address into R0. Its former void C definition could not supply that word to the two initializers. The guarded draft returns 0 explicitly. |
| `54B64 HitSwitch_AddFromScript` | 28 | The wrapper forwards the incidental R0 of the void 668-byte initializer at `54834`. Retaining the small wrapper's ASM is the smaller correction. Its guarded draft returns 0 explicitly. |
| `51B70 ProximityEmitter_IsTetherNearby` | 104 | In bounding-square corners outside the circle, the ROM returns the nonnull tether-position pointer. Four callers consume the result. The guarded draft returns that word explicitly, preserving the observed nonzero quirk. |
| `0821FE40 Collider_Refresh` | 44 | The ROM pops its saved Thumb return address into R0. Shared and local declarations now expose the actual word ABI. Its guarded draft returns 0 explicitly. |
| `66DB8 ActorAttachment_UpdateFromDescriptor` | 472 | Real callers `49258` and `4AB40` pass kind 127 and store the returned pointer. Unsupported kinds return the original slot with frame 127, or the final parts-release result otherwise. The complete defined draft differs in allocation and switch lowering. |

These five boundaries total **700 bytes**, excluded from exact C credit. Each
has a scored `NONMATCHING` draft and its original ASM fallback. The explicit-zero
drafts preserve observed success signs without claiming equivalence for the
ROM's incidental return-address bits. A builtin return-address attempt also
differed and was rejected.

The attachment draft accepts word arguments and explicitly narrows kind/frame
to 8 bits and count to 16 bits, as the ROM does. Null descriptor, release-only
null slot and all supported constructor/release paths return explicitly. A
kind 0–5 precondition would contradict the observed callers. Its complete
472-byte candidate has 12 differing bytes; no opaque result capture is used.

## Other return warnings

The reviewed results of `506B8`, `5173C`, `56DBC` and `5C8D8` are discarded task
callbacks. The sole callers of `6374C` and `61CA8` overwrite R0 immediately.
No consumed missing return value was established for these six entries.

`612D0 SceneInventory_ClassifySelectedAction` has an observed category domain
of 0, 1 and 2; those paths return explicitly. Its caller `61328` consumes the
result, but no reachable unsupported category was established. That input
domain remains a precondition, rather than a claim that all integer inputs
have defined results.

The separate 2,484-byte retained-tail accounting correction is documented in
[the subsystem report audit](REPORT_SUBSYSTEMS.md).


Final batch validation confirms all changed exact extents and all scored drafts
against the final ELF. The complete ROM is byte-identical; RAM-literal audit,
source diagnostics review and reporting tests pass.
