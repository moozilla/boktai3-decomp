# Player and minigame source recovery

Tags: #player #minigames #types #matching #phase3 #tooling

The lane-wide linter started at 402 weighted warnings and 393 steering-debt
uses. At the spiral trail checkpoint it reports 302 warnings and 293 uses, a
25.4% debt reduction. All 68 changed source units passed the stock ROM check.
The dependent checks covered every changed header. The two existing volatile
declarations in the target registry still contribute the lane's two errors.

## Typed objects and state updates

- All six Player attack trail emitters (081608B0–08161F18) use the attack
  particle task, its graphics array, and the owner's body AnimSprite. The
  creation path passes Player.animation at 0815F2C8. The allocator's
  ContactPosition accounts for the aligned position previously represented
  as a signed or unsigned bitfield record. The typed fields also clear 49
  linter uses attributed to address macros hiding runtime field reads.
  All nine attack-particle header dependents match.
- The Coffin Bike model at 0x610 uses CharacterPreviewModel, including its
  five parts, graphics resource, palette banks, appearance handles, and
  player index. At motion offset 0x68, the palette state starts with the
  same damage words used by ActorStats. Its palette index and target
  replace the separate recovery-value and parts-pointer views. The 150
  shared modifier-header dependents and 27 Coffin Bike dependents match.
- Eight Coffin Bike units use the combined engine and scene pause query
  already documented by the flag-order survey. This clears nine staged
  uses. The pending and delayed script callbacks use the same full bike
  type. The health save and restore functions read the participant's
  style block and the existing game-state field. The save computes the
  destination before loading the style word, as in the ROM.
  [Flag-order survey](flag-accessors_2026-11-25.md)
- DarkGenerator_UpdateInteraction (081C1FC0) updates the Player phase and
  resets its interaction fields directly. Two unrelated meanings of four
  no longer share a local. The unit matches with eight fewer constant uses.
- PlayerClone_InitSlot (08200A10) uses ContactPosition for the four collider
  bounds and writes direct reset values. Its addressable word is the source
  of a fixed two-word CpuSet fill. This removes 22 staged uses.
- The orbit effect uses AnimSprite and AnimSpriteResource for the same
  drawable initialized and stepped by the transition callbacks. Its
  position copies use ContactPosition. The initialization and reveal
  callbacks at 0815FEB0–08160060 use the shared task and its collider.
- The spiral trail is one 0x288-byte task with a 0x70-byte sprite followed
  by the emitter and eight 0x3C-byte slots. The slot initializer
  (08236DC4) indexes the array and writes the real fields. The emitter
  (08236C04) uses the task for the graphics load and final active store,
  keeping the parent pointer live alongside the emitter pointer. All
  11 trail task dependents match.
- The later target and collider branches that reuse a flag word can read
  the field directly. The first generic helper at the remaining sites
  still shapes mask setup. The debris sprite hide bit can use the bit
  already set up by the display-state test or update.

## Tool changes and repeatable checks

The family scan found 29 initial families and 61 embedded-type observations.
All families received a structmerge preview. Gated merges and follow-up
source checks recovered shared particle records, player stat fields, and
minigame records. The mechanical passes tried three raw-field rewrites and
55 single-use folds. Layout measurements and call evidence drove the matching type recoveries.

Structfamilies recognizes a known callee's scalar record when a private
whole-byte bitfield view has identical named storage and alignment.
Structmerge accepts an explicit larger shared prefix for pointer-only
uses, with measured member mapping and the usual compiler gate. It also
maps a private aggregate field to a matching measured nested field.
All 28 tests in tests/tools/test_structfamilies.py passed.

## Open decisions

- TargetRegistry_RegisterActor (0815F6A8) and
  TargetRegistry_RegisterSceneActor (0815F6F0) each match at 72 bytes.
  Removing volatile still needs an ordinary insertion form that preserves
  the global reload after a store through the first argument. The direct
  flag writes also reverse the ROM's mask and field setup.
  [Previous alias and control-flow experiments](hard-sites_2026-11-23.md)
- CoffinBike_UpdateRecoil (081188F4) builds the 0x3000 heading threshold
  before calculating several motion field addresses. Folding the literal
  moves the two construction instructions five instructions later.
  MotionActor_RunPendingScript (08116D14) also needs the wide mask and
  reset-store order. The direct reset narrows the mask to 0xFE and changes
  the reload rotation for the last flag update.
- Player_GetForceDirection (08140F40) shows 58 ROM / 56 draft differing
  instructions with a direct return in place of the nested wrappers.
  Giving the direction pseudo priority over short-lived terrain values
  isolates the remaining differences to the tail. Production C for that
  allocation and tail is still open.
- The orbit burst initializer (08160108) still holds a private particle
  view. Both indexed and typed pointer-walk conversions change the
  pointer allocation. The spiral trail slot dispatcher also keeps its
  existing pointer walk after direct callback accesses changed the loop.

The worker report under build/REPORT.md has per-family refusals, checks,
and resume locations. The integration run owns combined ROM verification.
