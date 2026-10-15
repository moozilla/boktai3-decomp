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
|0808148C|0808170C|640|Semantic draft: stagger burst|
|0808170C|08081B9C|1168|Assembly, review in progress|
|08081B9C|08081C98|252|Exact, register-pinned: resume after animation|
|08081C98|08082184|1260|Assembly, review in progress|
|08082184|08082444|704|Assembly, review pending|
|08082444|08082648|516|Assembly, review pending|

The section contains5048 bytes. Its two exact callbacks add approximately
**0.0315 percentage points** of matching progress:760 new bytes, all in the
Register-pinned category, with no previously credited bytes counted again.
The remaining4288 bytes do not count as exact progress. Full reviewed extents,
including literal pools, were used for both exact checks. Standard hint removal
reduced81290 from36 to18 hints and81B9C from75 to31. Parent batch integration
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

## Stagger burst draft, 0808148C

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
assumed neighboring callback table layout. This source remains private until its
whole640-byte extent matches and its ordinary C lifetimes have been audited.

## Compiler evidence

81290 initially had correct body instructions but an expanded conditional branch.
Removing empty register-hint assembly changed the compiler's estimated block
length and restored its short branch, even though those hints emitted no physical
instructions. Near-match hint reduction first reached a full exact baseline;
the standard depin pass then removed further hints while requiring exactness.
This is the same estimated-length effect seen in the earlier706DC/grid cohort.
