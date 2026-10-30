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
