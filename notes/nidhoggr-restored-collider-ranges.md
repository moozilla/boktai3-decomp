# Nidhoggr restored collider ranges

Tags: #matching #nidhoggr #register-allocation

**Nidhoggr_RestoreFirstPartRanges (08194858).** All 244 bytes match without a register pin.
The selected fixed-point bounds and offset apply to both the body and attack collider.
The capture-and-copy operation keeps the bounds source in r4 for the second copy. The
previous separate assignments put the actor in r4 and the bounds pointer in r5.
