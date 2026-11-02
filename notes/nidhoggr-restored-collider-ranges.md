# Nidhoggr restored collider ranges

Tags: #matching #nidhoggr #register-allocation

**Nidhoggr_RestoreFirstPartRanges (08194858).** All 244 bytes match without a register pin.
The selected fixed-point bounds and offset apply to both the body and attack collider.
The capture-and-copy operation keeps the bounds source in r4 for the second copy. The
previous separate assignments put the actor in r4 and the bounds pointer in r5.

**Nidhoggr_RestoreSecondPartRanges (0819494C).** The companion also matches all 244 bytes
without its r4 pin. It selects the complementary collider pair for each background mode
and reuses the same range operation.

**Nidhoggr_UpdateLeftwardRetreat (08196A5C).** The retreat walks 26 body colliders
from slot zero and 26 alternate colliders from slot one. A zero-initialized cursor
supplies the first body index and holds the alternate flags offset (0x5E56).
As a scalar, agbcc combines this offset with the actor address and reloads it into
r0. Writing the second field of the cursor keeps the offset separate through combine,
so the compiler loads it into r1. All 328 bytes match without either register pin
or the empty assembly barrier.
