# Nidhoggr restored collider ranges

Tags: #matching #nidhoggr #register-allocation

**Nidhoggr_RestoreFirstPartRanges (08194858).** All 244 bytes match without a register pin.
The selected fixed-point bounds and offset apply to both the body and attack collider.
The capture-and-copy operation keeps the bounds source in r4 for the second copy. The
previous separate assignments put the actor in r4 and the bounds pointer in r5.

**Nidhoggr_RestoreSecondPartRanges (0819494C).** The companion also matches all 244 bytes
without its r4 pin. It selects the complementary collider pair for each background mode
and reuses the same range operation.
