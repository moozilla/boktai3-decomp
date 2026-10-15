# Actor reaction transitions, 08081290–08082648

This contiguous callback subsection follows the reaction callbacks ending at
08081290. Actor and drawable interpretations remain tentative gameplay names;
field offsets and observed transitions are the contracts. It reuses
`scene_actor_reaction.h`, including the reset-phase actor prefix and embedded
control at420. No global actor layout changes are introduced by this cohort.

## Reviewed boundaries and exact progress

| Start | End exclusive | Reviewed bytes | Status |
| --- | --- | ---: | --- |
|08081290|0808148C|508|Exact, register-pinned: sink and remove|
|0808148C|0808170C|640|Exact, register-pinned: stagger burst|
|0808170C|08081B9C|1168|Assembly, semantics reviewed|
|08081B9C|08081C98|252|Exact, register-pinned: resume after animation|
|08081C98|08082184|1260|Assembly, semantics reviewed|
|08082184|08082444|704|Private semantic draft|
|08082444|08082648|516|Private semantic draft|

The section contains5048 bytes. Its three exact callbacks add approximately
**0.0580 percentage points** of matching progress:1400 new bytes, all in the
Register-pinned category, with no previously credited bytes counted again.
The remaining3648 bytes do not count as exact progress. Full reviewed extents,
including literal pools, were used for all exact checks. Standard hint removal
reduced81290 from36 to18 hints,81B9C from75 to31, and8148C from27 to21. Parent batch integration
performs the combined build and ROM comparison.

## Sink and remove, 08081290

The callback consumes and clears prepared byte2B2. On entry it takes category98,
sets collider134 flag4, shows the drawable, selects directional animation3, and
resets the animation state to3/0/0 with counter2AC zero and byte2B1 set. It clears
AA,2B3, collider134+44 and2B8, then sets flags2D0 mask0x20000. If actor word0 is1,
it first clears the shared route flags and control counters.

Attachment creation C72C(kind0,frame1,category) is skipped for categories2 and22.
This exclusion does **not** prove that every remaining category is supported by
C644. The earlier C72C near-match remains unpromoted for the precondition issue
documented in `SCENE_ACTOR_ATTACHMENT_RESPONSES.md`.

For signed frame>6, Y decreases by30*(frame-6), truncated to its halfword, and the
whole eight-byte position is copied to the drawable. Sequence drawables receive
it at1C; tile drawables at40. Source arithmetic uses an unsigned product to retain
the observed wrapping behavior without signed multiplication overflow.

For frame>8, health becomes zero; flags2D4 mask8 and flags2CC mask0x400 are set.
Attachment kinds0,1,3 are released through the frame127 C72C path, E7B0 receives4,
and substate23 is installed with callback08080FB4. The common control installation
also sets prepared2B2, clears2B0/2C8/104 and flags114 bit1. The drawable is hidden:
word0 bit1 for sequence drawables, or word28 bit1 for tiles. These are observed
state changes; no broader death/destruction identity is asserted.

## Resume after animation, 08081B9C

On prepared entry it selects animation7, calls080771C4, and clears flags2D4 mask2.
Once byte2B3 is set, it clearsAA, sets2B0 and flags2CC mask0x10, then selects
animation0. It resets animation state3/1/0 and restores the control pointer at
actor5E4 through substate15. Control installation finally clears2B0 again; the
first write precedes an external animation call and is preserved.

The second incoming word is unused by this callback. Actor storage, the selected
drawable representation, and the stored control pointer retain their existing
caller contracts; no invented null fallback is added.

## Stagger burst, 0808148C

Prepared entry selects animation3; category22 uses08076818 instead of08076034.
It shows the drawable, resets animation state3/0/0, clears2B3 and flags2D4 mask0x20,
calls08077B1C, and plays song298. It initializes the named X/Y/Z components of a
packed vector to60/60/40, advances the explicit random-table index once modulo
1024, and calls0806A160 with mode1..4, heading direction+128, and trailing operands
10,6,2,2. Aggregate padding is not read as a C scalar or assigned an invented
value. The callee's own random effects are separate from this explicit advance.

Every update clears the drawable animation-frame halfword: sequence state48+E
or tile2C. At signed frame>15, flags2D4 mask2 chooses the return transition.
The flagged path resets3/0/0 and installs control actor5DC with substate7; the
other path installs actor230 control with substate18 and then resets3/0/0.
These offsets follow complete immediate arithmetic in the ROM, rather than an
assumed neighboring callback table layout. The complete640-byte extent is exact; initialized scalar lifetimes and the
packed-vector aggregate convention were audited before promotion.

## Compiler evidence

81290 initially had correct body instructions but an expanded conditional branch.
Removing empty register-hint assembly changed the compiler's estimated block
length and restored its short branch, even though those hints emitted no physical
instructions. Near-match hint reduction first reached a full exact baseline;
the standard depin pass then removed further hints while requiring exactness.
This is the same estimated-length effect seen in the earlier706DC/grid cohort.

## Motion settling, 0808170C — assembly retained

Prepared entry rounds facing to a cardinal direction and selects animation6.
Unless flags2D0 mask2 is already set, it creates attachment kind3 with frame2
when mask1 is set (then clears mask1), or frame0 otherwise, then sets mask2.
Category sounds are observed for1(song254),3(variant-event helper with2),4(song237),
5/6/7/21(song316), and25(song460). These category switches still do not prove
that every C644 creation input is supported.

The callback preserves repeated flag/timer resets: flags2D4 is ORed with2 and
masked by0xE856,2C2 is set64,414/418 are cleared, flags2D4 masks0x380/0x400 are
cleared,5DA becomes255, and actorEB masks0x18 are cleared. A flags2CC0x200000
path additionally resets control mode/counter/timer and clears flags2D0 bit24 and
flags2D4 mask0x200. The shared route reset follows the repeated clears. These
writes should not be collapsed across future external calls without checking
code generation and aliasing.

For signed AA in30..119, the Mod/Div expression
`AA % 30 < 15 - AA / 10` sets flags2D6 mask0x20. The callback also examines a
sequence drawable's state8==2, byte7, and frameE, clearing frameE for the observed
`frameE == !!byte7 && byte7 != 1` condition.

Two signed halfword pairs310/318 and314/31C are eased toward each other. Each
quarter difference truncates toward zero and is clamped to[-200,200]. The
increments are added to motion64/68 and to318/31C, with halfword truncation.
When both increments are zero and AA<=0, AA is cleared,2B6 is derived from facing,
and callback5E0/substate8 is installed with animation state3/1/0. It clears
flags2D0 mask2 and flags2D4 mask0x3A, rounds facing again, then enables collider134
mask0x100 only if flags2C0 mask0x40 and flags2D4 mask0x2016 are both clear.
The complete1168-byte body has been semantically reviewed but is not exact C.

## Timed particle response, 08081C98 — assembly retained

Prepared entry rounds facing, selects animation3, and performs the related
route/flag resets. It initializes the effect config at47C, calls E6FC with
operands3,2,10,20,3600, sets flags2CC0x20000000, and writes flags2D0 as
`(old | 0x4000) & ~0x140`. The following palette branch tests0x4000, choosing
halfword1DC or1E4; sequence drawables call the palette API on drawable2C, while
tiles receive the palette base plus bank*32 at drawable68. The apparently
redundant flag test is present in the ROM and is not evidence of a separate
external transition.

The unsigned timer2B8 is compared with timer5CE and twice timer5CE. Either equality
emits one particle from config47C/origin484, with origin Y+128, random jitter
X/Z[-7,8],Y[-16,15], vector5/10/5, and trailing40/40. This consumes three explicit
random-table advances and sets flags2D6 mask0x20. Timer<=10 also sets that mask;
field1EA is written65 every update. A nonzero timer decrements.

At timer zero, collider134 mask0x100 is enabled. If flags2D4 mask0x2000 is set,
three further particles are emitted (nine explicit random advances), and AA/2B8
are cleared. The callback clears mask0x2000, resets state3/1/0, and resumes
actor5E4 control with substate15. Finally2B6 is derived from facing+128, truncated
to a byte. The complete1260-byte body remains assembly with no C matching credit.

## Route resumption draft, 08082184

Prepared entry derives facing from2B6, selects animation0, and clears2B3. At
signed frame>60 with2B3 set, it sets2B0 and requests attachment kind0/frame2 for
the actor category. It copies X/Z from the point pointer450 into destination5D4,
then calculates terrain Y with slope corrections. Context5D0 tests that position
through F204/F7A8. Failure clears flags2D4 mask0x200. Accepted result2 preserves
motion; other accepted results clear the eight-byte motion64 aggregate.

Failure installs FC90/substate0 and state3/0/0. Success marks the destination's
fourth halfword1, selects state3/1/0 and FDC0/substate14, sets flags2CC0x10 and
2C2=64, then writes the embedded response468..470 as state0/phase0/behavior7,
threshold60,control228. Valid route, destination, and geometry storage remain
caller preconditions; the draft adds no fallback positions or invented bounds.
Private source is `wip/sub_08082184.c`; the whole704-byte target is not exact.

## Turn-response restoration draft, 08082444

Prepared entry selects animation0, sets flags2D4 mask0x400, and sets2C2=32.
Signed byte5DA is the remaining-turn count. While positive and frame>60, the
callback selects state3/1/0 and control224/substate2. On the last turn,3CD is
`((savedFacing5D9 >> 5) + 3) & 7`; otherwise it samples cardinal directions
0/64/128/192, retrying equality with facing up to four attempts, then encodes
3CD the same way. The byte count decrements. The explicit RNG advance count is
therefore between one and four on that sampled path.

A nonpositive count is normalized to255. After signed frame>30, callback460 and
saved substate467 are restored, followed by state464/phase465/behavior466,
counter2AC zero and2B1 set. Flags2D4 mask0x400 is cleared. This links directly to
the temporary response packet written by the earlier DB34 cohort. Private draft
`wip/sub_08082444.c` covers all516 target bytes but does not count as exact.
