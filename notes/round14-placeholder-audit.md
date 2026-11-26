# Round 14: placeholder-name audit

Tags: #matching #actors #animation #tooling

## Scope and result

This pass audited all 24 `placeholder-name` sites in 18 translation units.
The source baseline was `3af1eae410b915dae0dbe7bbdaa90a9aadfe0a9b`, after the
constant-local batch and both diagnostic corrections. Vanargand's pending-state
access used the separately integrated `e09d202e6` prerequisite.

**16 sites are resolved; eight remain visible in the source.** Fifteen flagged
declarations were removed. The sixteenth was the genuine returned source-position
pointer in `PreviewActor_ReplaceProjectileTowardPlayer`: it now has the shared
position type and the name `source`, while both artificial destination aliases,
the word-view copy helper, and the unused union were removed.

The compiler-aware scan over these same 18 units changed as follows:

| Kind | Before | After |
| --- | ---: | ---: |
| `placeholder-name` | 24 | 8 |
| `const-local` | 19 | 19 |
| `local-union` | 2 | 2 |
| Errors | 0 | 0 |
| Total warnings | 45 | 29 |

There are no new warning kinds or sites and no new ignores. The unused
file-scope `PreviewPosition` union was removed; it was not one of the two
`local-union` warnings in this table. The nine corrected compiler diagnostics
and the earlier declaration-scanner correction are already in the baseline and
are not credited to this source pass.

Every changed unit received an independent stock `tools/check.py` check with a
reviewed target extent. The 12 changed units contain 77 functions and 22,516
bytes, all exact. No full build or progress-report update ran in this worker.

## Accepted source changes

Worker commit IDs are listed below; integration may cherry-pick them under new
IDs. Hresvelgr has two source commits because the zero and duplicate actor
pointer were independent removals.

| Unit | Source change | Placeholder sites | Whole-unit bytes | Worker commit |
| --- | --- | ---: | ---: | --- |
| `src/objects/items/sprite_drop/sub_08115F74.c` | Advance the actual callback pointer without assigning an unused second array-base alias. | 1 to 0 | 56 | `428cee113` |
| `src/player/motions/sub_08149A14.c` | Remove the unused `dummy` declaration. | 1 to 0 | 988 | `64bd7f0ee` |
| `src/bosses/hresvelgr/sub_081A117C.c` | Store the motion zero directly and use the existing actor view. The pointer-to-pointer alias remains open. | 3 to 1 | 200 | `a21e52f43`, `a083a0423` |
| `src/bosses/vanargand/sub_081ACAC4.c` | Write the two declared delay fields directly, retaining their existing common halfword delay value. | 1 to 0 | 156 | `6dc3d96d1` |
| `src/ui/books/monster_book/sub_081D46B4.c` | Recover origin and direction fields from matched relatives; remove the cached direction and destination pointer. | 1 to 0 | 152 | `a747f40c3` |
| `src/effects/projectiles/projectile/sub_081D76F4.c` | Use `ContactPosition` and the actual entry fields; remove the position alias and the literal shared between random range and address arithmetic. | 3 to 1 | 384 | `11bfe1fe4` |
| `src/bosses/ratatosk/sub_081F9E20.c` | Use the shared projectile parameter/position contracts, actual position pointers, and direct aggregate copies. | 3 to 0 | 240 | `77aa80ad3` |
| `src/system/inventory/sub_0822D080.c` | Use the existing slot parameter in the packed-state exchange. | 1 to 0 | 72 | `60def4a42` |
| `src/link/player/sub_08241BC4.c` | Remove the unused `dummy` declaration. | 1 to 0 | 756 | `b67684658` |
| `src/link/player/sub_08242EA4.c` | Store the computed flags without the assignment-only field pointer. | 1 to 0 | 216 | `1b631516e` |
| `src/monsters/lun.c` | Read the typed participant position directly; remove both the placeholder alias and the separate unused `players` local. | 1 to 0 | 16,116 | `5a30e5054` |
| `src/scene/terrain/nav_mesh.c` | Reuse the existing `GetMesh` accessor and remove assignment-result base aliases from both the caller and accessor. | 1 to 0 | 3,180 | `63d034219` |

### Evidence behind the type and dataflow changes

The palette layout is already explicit in `PaletteTransition_StartDisplay`
(`081D48D8`) and `PaletteTransition_InitializeChainDisplays` (`081D3B58`): origin
is at `0x9EC`, direction components at `0x9FE/0x9FF`, and direction at `0xA00`.
Using those fields in `081D46B4` removes the need for a loaded-value placeholder.
No shared header was changed.

The trail entry's eight-byte position at `0x18` and signed angle offset at `0x29`
are established in `ProjectileTask_UpdateTrailSprites` (`081D75E8`). The existing
aligned `ContactPosition` type supplies the position copy directly. The old
`new_var3 = 0x60` served two unrelated roles: a random modulus and the address
of the first entry's position. The accepted source uses the literal only as the
random range and addresses the position through the entry field. The separate
heading-narrowing placeholder remains unchanged and warned.

`include/effects/projectiles/projectile_config.h` already declares the real `Task4E01Params` and
`ContactPosition` contracts. `ProjectileTask_GetEntryPosition` in
`src/effects/projectiles/projectile/projectile_config.c` returns the entry's `sprite.position`.
`OrbitEffect_FireProjectileOnContact` (`081FD704`) also gets a source position,
copies it to an origin or target, then calculates the firing direction. This
supports the remaining real `source` pointer in `081F9E20`; it is not a renamed
duplicate destination or an unused state variable. The final source removes
the private word-view helper and uses the actual shared aggregates.

The navigation change is both deduplication and removal. `GetMesh` already
existed for `NavMesh_UpdateContainingRegion`; its body also contained an
assignment-only array-base alias. Its revised body advances and reads the
actual serialized-offset pointer. Both callers match, and all 17 functions in
the translation unit were checked. A compact direct array lookup was also
tested and did not match; it is preserved in the trial record.

## Eight open sites

The following eight warnings remain allocation-related source work. Their
names were retained so they remain visible. The natural candidates below are
not production source and do not establish an exhaustive impossibility result.
Counts are differing ROM/draft instructions from the ordinary scope compile;
diagnostic knobs were used only to identify mechanisms.

| Function | Remaining declaration | Natural candidate | Difference |
| --- | --- | --- | --- |
| `ActorAttachment_UpdateValueFrame` (`08065904`) | `s16 new_var` | Load the signed-halfword value directly into `current`. | 43 / 41, plus two moved branches; 192 to 188 bytes |
| `ClockPanel_CaptureSnapshot` (`08109AEC`) | `unsigned int new_var = 16` | Address the second byte block directly. | 3 / 3; 228 bytes |
| `SceneActorPlacement_PlayResponseAndResetState` (`08136AF0`) | `u8 new_var` | Test the mask result without a narrowing assignment. | 2 / 2; 348 bytes |
| `Hresvelgr_UpdateRandomAnimation` (`081A117C`) | `struct ActorReaction **new_var3` | Access the existing actor pointer directly. | 10 / 10; 200 bytes |
| `Storage_BeginCategoryMode` (`081BF440`) | `int new_var = -1` | Pass the sentinel directly in the description call. | 2 / 2; 220 bytes |
| `Kuro_UpdateJumpToTarget` (`081D1E78`) | `struct Tagged *new_var` | Use the existing case-local tagged-state pointer. | 7 / 7; 420 bytes |
| `ProjectileTask_SpawnTrailSprite` (`081D76F4`) | `unsigned char new_var` | Mask the computed heading directly in the accepted real-field implementation. | 1 / 1; 384 bytes |
| `SceneActor_FindNearbyActivePlayer` (`src/monsters/mimic.c`, `080E7F54`) | `s16 new_var` | Square `delta->z` directly. | 7 / 7; 160-byte function |

### What the attempts established

**08065904:** The direct load and a genuinely narrow `current` both remove a
second signed-halfword load and change the allocation of `slot`, `level`, and
the loop's pointers. The direct-load candidate is 188 bytes versus 192 in the
ROM. `AGBCC_FORCE=level:3` improves the difference from 43/41 to 41/39; preventing
canonicalization of `slot` does not help. A bounded probe of nine CSE decisions
also did not match. The duplicate narrow copy remains open.

**08109AEC:** Removing the early literal reverses the initial snapshot/record
pointer copies between `r5` and `r7`, changing only three instructions. Walking
the destination pointer to the second block gives the same difference. Using
the declared `record` field and the existing `GameState.playerName` source
field was also tested; those natural forms produce 232 bytes. Scope cannot yet
trace the differing late copy to a pseudo or recorded reload decision.

**08136AF0:** The direct test ties the AND to the value register instead of the
mask register. `AGBCC_NOTIE=151` makes the diagnostic candidate exact, confirming
regmove's tie. The existing placement boolean helpers were tested in both
argument orders and through the actual motion-flags field; they do not match.
An explicit `u8` cast matches, but it merely retains the unnecessary narrowing
under another spelling and was rejected.

**081A117C:** The zero temporary and second actor pointer are gone. Removing
the remaining address-taken pointer alias changes the allocation of the actor
and tick and several related moves. The actor has 14 references over a
75-instruction live range in the direct candidate. The tested canonicalization
and tie knobs did not improve its 10/10 difference. No new replacement alias
was introduced.

**081BF440:** Direct or branch-local `-1` materializes `movs r3, #1; rsbs r3, r3,
#0` before the first two argument moves. The ROM materializes it after them,
immediately before the description call. The early constant is still exposed.

**081D1E78:** Keeping the existing case-local pointer moves `entered` from `r1`
to `r0`. Forcing `entered` to `r1` reduces 7/7 to 3/3 but does not match. Using
the common tagged pointer everywhere, or one consistent case-local view,
produces 408 bytes rather than 420. The extra pointer's lifetime still needs a
source-level explanation.

**081D76F4:** Removing the narrowing lets the first `#7` mask value survive the
following addition; the compiler omits the ROM's second `movs r1, #7`. A padding
instruction later compensates in total extent. Seven CSE/loop probes did not
improve the difference. The actual entry type and random-range/address issues
were resolved independently; this mask-reload issue remains.

**Mimic:** The direct square lets the compiler hoist `motion`, the address at
actor offset `0xDC`, out of the player loop. Scope reports the hoist test as
`26 * 1 * 2 >= 51`; `AGBCC_NOMOVE=175` makes the diagnostic candidate exact.
This is a loop-invariant-motion consequence of the extra narrow copy. An exact
candidate that renames that copy and spells out its multiplication was rejected
because it retains the same redundant narrow state. Direct coordinate products
were also tested and did not match.

## Retained evidence and tooling gaps

`notes/round14-placeholder-trials.jsonl` records 66 fixed-hypothesis stock
trials. Each record includes the base revision, hypothesis, source hash,
whole-unit result, and complete candidate patch against that base. The trial
record includes rejected exact candidates; `match: true` alone does not mean
the candidate was accepted. The accepted changes are the commits above.

`notes/round14-placeholder-natural-candidates.patch` retains one natural
nonmatching candidate for each of the eight open functions, against the
accepted source at `a083a0423`. Each file patch is an independent investigation
candidate. The scratch logs and compiler-warning summaries are under
`build/round14-placeholder/`.

Scope gaps observed in this pass:

- The late snapshot pointer copy in `08109AEC` has no pseudo/reload provenance
  in the current `why` explanation.
- The missing repeat mask load in `081D76F4` has no differing draft RTL UID;
  `why` can list it and run probes but cannot yet identify the lost materialization.
- Some Hresvelgr reload-created moves have no supported causal summary.
- Mimic's `hoist` analysis identifies the correct moved address, but its
  automatically generated lever candidate refers to block-local `motion`
  outside that variable's scope and fails to compile. The explicit diagnostic
  `AGBCC_NOMOVE=175` succeeds; that knob is not used by production compilation.

All eight remaining warnings stay open for further source/type recovery. No
suppression or renaming was used to make this queue appear empty.
