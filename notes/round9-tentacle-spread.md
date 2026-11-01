TentacleBody_UpdateSpreadRelease matches all 776 bytes without either barrier.
The spin is loaded before the direction branch. Its unsigned low byte feeds the
magnitude accumulator afterward, and the saved direction address is reused for
segment placement.

TentacleBody_UpdateChargeSpread now matches all 808 bytes without a register
pin or barrier. Grouping the segment's three offset assignments keeps the
segment-index calculation in r8 and the body pointer in r9. The same operation
is shared with the release callback.
