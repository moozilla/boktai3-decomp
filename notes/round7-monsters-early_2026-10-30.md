# Round 7 early monster matches

Tags: #monsters #ghoul #matching

## Ghoul_TurnTowardRoute (08086400)

Exact 1,580 bytes in ordinary C using the shared turn operations. Initializing the
body, route, and group owners in that order preserves the entry address setup.
Animation nine, frames one and three, and the 0x20 direction step are Ghoul-specific.

Validation: `tools/check.py src/fn/sub_08086400.c --target-size 1580 --json` reports
zero differing bytes.

## Ghoul_ResumeDelayedChase (080898FC)

Exact 2,344 bytes using canonical chase, saved-response, and state-entry operations.
The group owner is captured at entry; the zero result of the blocking query supplies
the later state reset. Stock `tools/check.py` reports zero differing bytes.

## Ghoul disc route recovery (0808A664)

Exact 524 bytes. Canonical state-entry, controller, delayed-response, and route
operations replace raw actor offsets. The motion-flags operation accepts its actor
owner so its mask is prepared before the flag address. The context view exposes
the disc targets and recovery delay. Stock check reports zero differing bytes.

## Bat landing recovery (08087018)

Exact 2,648 bytes. The landing-preparation operation retains the reaction flag
component while the caller handles recovery. The earlier draft incorrectly cleared
motion flags instead of reaction flags at 0x2D6 and fetched a group callback where
the ROM uses `entryTarget228`. Typed group ownership resolves the final PRE
first-discovery tie. This uses the existing typed-group interface selector; no
new selector or compiler override. Formatted stock check reports zero differing bytes.

## Bat displacement recovery (08087A70)

Exact 4,532 bytes using the verified Mummy displacement operations with Bat's
entry animation, sounds, and route-selection differences. Capturing the pursuit
group at entry preserves the complete allocation and PRE frame. The existing value/ownership
interface selections remain unchanged. Stock check reports zero
differing bytes after formatting.

## Slime scattered reaction burst (08090724)

Tags: 08090724, Slime, reaction, random, PRE

Matched 884 bytes of ordinary C using the already exact targeted burst's component interfaces. The random sample helper keeps the sample index distinct from its table address, retaining the index across the position subtraction. Its 32-bit sample result agrees with the callers' masks and gives the ROM's PRE-generated address order. Before the return-type correction, complete physical instruction offsets/opcodes/registers matched with only seven stack slots permuted; `scope frame` verified the bijection and the PRE hash model. Formatted source passes stock `tools/check.py src/fn/sub_08090724.c --target-size 884 --json`: zero differing bytes, SHA-256 `9bc9790a9edc791097afcd76b54be95098367ff27b6a61f53ba164af784fd41f`. No register pins or barriers.

## Ghost curve step (08097798)

Tags: 08097798, Ghost, curve

Matched 436 bytes using the established Lun curve interpolation operation and canonical actor context owner. The private curve record exposes only the phase, tangent scale, output, start, and end fields. The ordered halfword math and signed truncation are unchanged from the shared behavior. Formatted stock check reports zero differing bytes, SHA-256 `963d02214ce4393aa7da50b92ab8cee08c9139b25f773dd5cef817daf8a5a3c7`.

## Ghost curve route points (08098344)

Tags: 08098344, Ghost, curve

Matched 488 bytes by reusing the exact Bee route generator with the Ghost point storage and motion scale. Canonical actor position and context access replace the opaque draft. Stock formatted check has zero differing bytes, SHA-256 `1d72275f6d28ab0474c0f38b5c51de478976079d70945148353e88837be83bc7`.

## Ghost drained-player follow (08098A74)

Tags: 08098A74, Ghost, reaction, PRE

Matched 556 bytes without the old r6 pin. The entry operation owns initial control, player-position capture, flags, and heading; it returns the actor motion-flag component used by animation scaling. This places the shared player-slot address before the returned component address in PRE, resolving the final five-instruction ordering hunk. Context position, player position, and player drain-stat accesses now use typed fields. Formatted stock check has zero differing bytes, SHA-256 `f7f2fc0c21e2b5218171ee177f4c1f95f317563c9f3dfdeeb5621c5c22620ef2`.

## 080A9BD8 — exact, 472 bytes

- Reused the verified Ghost/Lun position interpolation operation for the segment range. Kept caller-owned zero tangent scales and the separately evaluated fixed phase range, matching the original curve arithmetic and loop strength reduction.
- Canonical actor with typed private trail segments; no pins, barriers, compiler switches, or assembly.
- Formatted stock check: MATCH, 472 / 472 bytes, zero differing bytes. Evidence: `build/round7/early/080A9BD8/stock-check.json`.

## 080AA240 — exact, 740 bytes

- Reconstructed oscillation and midpoint interpolation using canonical actor, typed private trail/drawable components, and the verified curve operation.
- Recovered the previously unlabeled trailing helper at 080AA4B8 (108 bytes) as an ordinary static C function in its original position; the reviewed span includes both functions.
- Byte flag mask ownership hoists the full keep-mask in the first loop pass, giving the ROM allocation naturally.
- Formatted stock check: MATCH, 740 / 740 bytes, zero differing bytes. Evidence: `build/round7/early/080AA240/stock-check.json`.

## Tentacle segment impulses (080AAA90)

Tags: 080AAA90, Tentacle, impulse, loop

Matched 348 bytes with canonical actor ownership and typed position/velocity fields. The segment flag query accepts its segment and a byte mask; removing the draft's unused segment-address calculation restores the original three induction values and mask hoists. Context acquisition follows the inactive-actor gate. Formatted stock check reports zero differing bytes, SHA-256 `d4a59f7a0e661fb725d45763b4b9e23001896678df19ba457104d03920ee4ecf`. No register pins or barriers.

## Tentacle random segment impulses (080AA744)

Tags: 080AA744, Tentacle, impulse, random, allocation

Matched 384 bytes with typed trail segments and canonical actor motion. The caller owns the active/hidden flag masks, making the segment queries share the intended word-mask interface. The random operation returns its unsigned halfword sample; this preserves the original loop-invariant selection for the sine table and angle masks. The last blocker was proved by one reload-rotation knob before the source ownership/type fix. Formatted stock check reports zero differing bytes, SHA-256 `256c2d936f7c21cb5dc2f580a0e145fa4048ed732d3593637d33defcc58eff96`. No pins, barriers, or compiler overrides.

## Tentacle return to cell center (080A8280)

Tags: 080A8280, Tentacle, motion, PRE

Matched 828 bytes. Signed acceleration replaces the draft's incorrect byte cast; joined signed-truncation and wrapped-heading operations recover the original result copies. Removing the draft's redundant motion pointer and its reassignments lets PRE retain the single canonical motion address. Typed context exposes the starting cell and center, and canonical callback/drawable/body members replace actor offsets. Formatted stock check reports zero differing bytes, SHA-256 `eb738d190fea516b2caacc54a6cb1c5514055572464f361d2df65d0977ab219e`. No pins, barriers, or compiler overrides.

## Bee movement curve (08093138)

Tags: 08093138, Bee, curve, combine

Matched 476 bytes by reusing the established curve interpolation and typed private context. Updating the halfword time directly with the signed random input preserves the original constant copy and addition; the draft step local made combine replace that addition with OR. Formatted stock check reports zero differing bytes, SHA-256 `a6ce6ee4add39168f1c2ddf0d47cc35914297b59002e1487fc5c941305d4c06b`. Named the routine `Bee_AdvanceMovementCurve` and updated its caller.

## Tentacle reaction convergence (080A71A0)

Tags: 080A71A0, Tentacle, reaction, PRE

Matched 532 bytes with typed trail health/count and canonical actor motion. Joined signed truncation preserves both shift-result copies; shared flag components retain the masks and separate zero values used by control resets. Halfword flag masks and motion-clamp bounds resolve the final PRE slot permutation, confirmed against the compiler hash model. Formatted stock check reports zero differing bytes, SHA-256 `03ebcfafb2ee382a44b0611c97079b11fb2346ffa11723d28d0190bf84445e69`.

## Ghost visibility controller (08098788)

Tags: 08098788, Ghost, visibility, flags, PRE

Matched 628 bytes with canonical actor/player/game-state access and a typed private visibility context. The discarded engine flag query uses a boolean expression return; the early-return form had removed the ROM's dead masked read. Joined visibility-query exits retain both drawable flag tests, and separate context flag clear/set helpers let PRE keep the shared flag address. Formatted stock check reports zero differing bytes, SHA-256 `24a7bae007b90bbc9dc8f55f965931f57eb6ed7768942bed98f3a89c8ec0f484`. No pins, barriers, or compiler overrides.
