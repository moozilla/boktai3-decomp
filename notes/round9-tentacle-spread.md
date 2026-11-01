TentacleBody_UpdateSpreadRelease matches all 776 bytes without either barrier.
The spin is loaded before the direction branch. Its unsigned low byte feeds the
magnitude accumulator afterward, and the saved direction address is reused for
segment placement.

TentacleBody_UpdateChargeSpread matches all 808 bytes with the same scale
helper. Its mask barrier is gone. The entry clobber on r8 remains.
