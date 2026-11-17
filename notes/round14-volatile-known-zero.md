# Round 14: four further known-zero functions remain open

Tags: #round14 #concessions #matching

This follows `round14-volatile-stage2.md`. The assigned follow-up was
`TetherActor_UpdateBounceReturn`, `Spider_UpdateWanderGoal`,
`TargetRegistry_FindById`, and `GameState_SelectRamBase`, examined in ROM order.
No source changes, ignores, shared-header changes, or diagnostic changes are
included in this follow-up. All five genuine compiler warning sites remain
visible. The additional impossible collider-ID test discussed below remains
open too, although the current compiler warning does not report it.

These are unresolved source-recovery decisions. A byte match with the current
known-zero expressions is not evidence that those expressions are acceptable
original C. The prior commits explicitly described them as allocation-only:
`bce1d1dd4`, `dd59d2993`, `db6be2d3a`, and `a49798198`, respectively.

All comparisons used the pinned stock Thumb agbcc through `tools/scope/scope.py`.
The diagnostic compiler's allocation explanations and what-if controls were
used only to identify the remaining decisions. They are not build options or
admission reasons. Scratch candidates are under `build/round14-zero/`.

## 0804CB4C: TetherActor_UpdateBounceReturn

The current 356-byte match constructs the random table with
`RANDOM_TABLE_ADDR + (actor->stateFrame >> 16)`. The frame field is `u16`, so the
added term is zero. Its actual effect is to delay recognizing the table pointer
as a constant; this avoids the constant-register priority reduction and lets
the table take the register needed before the frame address.

Natural attempts:

- A direct numeric pointer and the existing `gRandomTable` symbol both leave
  21 ROM / 16 draft instruction differences. The draft keeps the frame address
  in `r6`, where the ROM retains it in `r8`; that also changes later reloads.
- Existing `ParticleRandom_ReadSample` and `ParticleRandom_NextSample` forms
  leave 32/29 and 38/33 differences, respectively. Explicit ordinary draws
  with a `u16` sample reproduce 21/16; `u32` and `s32` samples leave 27/27.
- A real cached `u16 *frame`, following the neighboring state handlers' field
  pointer pattern, gets to 6/3. It computes the frame address earlier and
  removes a halfword conversion pair that is present in the ROM. Moving its
  initialization to the later bounce block returns to 21/16.
- A conventional no-argument random helper gets to 9/11, but loads the table
  later and retains the first cursor store. Giving that helper a local table
  before advancing the cursor returns to the constant-allocation mismatch.

No alternative known-zero expression, dummy use, or new random-header
scaffolding was substituted. A faithful random macro/helper or genuine frame
access pattern remains to be recovered.

## 080A4018: Spider_UpdateWanderGoal

The current 584-byte match hides the table, cursor address, and 1023 mask behind
the high half of a value truncated to `u16`. It also adds zero terms involving
the mask and cursor to the final flag test. The compiler reports two sites;
that count does not mean there are only two artificial zero operands.

Removing all these terms while retaining the current control flow leaves
27 ROM / 29 draft instruction differences and a 592-byte draft. Scope identifies
the table and mask as constant-valued pseudos whose allocation priorities are
halved. The ROM keeps the table in `r6`, the mask in `sb`, and the cursor in
`sl`; the direct form assigns the low register to the heading, keeps the mask
in `r7`, and keeps the distance in `sb` instead of the ROM's stack slot.

Natural attempts included:

- All three draws through the existing `ParticleRandom_NextSample`: 27/29.
- The existing direct random helper and Spider's existing private `Random`:
  42/46 each. The separate-sample helper leaves 60/57.
- Existing `SceneActorBody`, `RoomActor.reactionStart`, and chase-group route
  fields in place of the private body/goal/group views: 42/38.
- The actual `u8` return type of `Chase_TryRoute`, the existing shared flag
  predicate, and direct body fields were also checked. None matched.

The source retains both warnings. No extra use of the random state was added
to preserve its lifetime, and no shared random helper was altered to conceal
the constant identities.

## 0815F5B4: TargetRegistry_FindById

The current 68-byte match has two distinct artificial constructs: the reported
registry pointer OR zero, and a negative test of an ID loaded from the `u16`
`ContactCollider.collisionId` field. The latter never succeeds; its assignment
to the requested ID is an unreported register-only dead arm.

Removing both gives another 68-byte function with only five differing
instructions, all register substitutions. The requested ID moves from the
ROM's `r3` to `r4`, and the cached target-kind flag moves from `r4` to `r3`.
The diagnostic priority experiment confirms that this is an allocation choice;
it is not an acceptable source fix.

Natural attempts included separate ID comparisons for the two target kinds,
early null returns, ordinary while/do loops, caching the invariant flags before
the loop, and testing the disabled flag at its point of use. Separate
comparisons correctly add real uses of the requested ID, but produce 13/13
differences: its priority becomes too high and the actor-ID arm joins the
comparison before the return rather than after it. The early-do and cached-flag
forms leave 15/13. No private or shared registry layout was changed.

The ROM deliberately repeats the same entry on an ID mismatch. Every attempt
preserved that behavior; adding a missing list advance would be a behavior fix,
not a matching decompilation of this routine.

The pointer identity and impossible ID arm remain open. Shared registry types
are being investigated separately for the neighboring functions and may provide
better evidence for this routine later.

## 0821B084: GameState_SelectRamBase

The timer register is a legitimate volatile halfword read. The artificial part
is using `timer >> 16` to clear bits from the backing-store address. That term
is always zero because the load is 16 bits wide.

Removing the address mask gives an 88-byte draft, the same size as the ROM,
with exactly one differing instruction:

- ROM: `adds r1, r1, r0`
- Draft: `adds r1, r0, r1`

The real decision is the operand order of the base-plus-jitter addition. The
direct symbol, numeric backing-store address, integer-address local, `u16` or
`s32` timer local, and a `u16` byte-offset local all leave the same single
difference. A naturally indexed array of eight-byte blocks and explicit byte
index/offset-before-base forms change allocation more substantially and do not
match.

The hardware read was not made nonvolatile, and no artificial store, identity,
or alternate address mask was introduced. The genuine known-zero warning
remains open pending an original address-calculation form that preserves the
operand order.

## Accounting

This follow-up lands zero source fixes and zero shared-helper deduplications.
It removes zero diagnostic sites. The five reported sites above, and the
unreported collider-ID dead arm, remain explicit open work. Root's separate
correction for spurious warnings from rejected compiler combine attempts is
diagnostic work and is not counted as source cleanup here.
