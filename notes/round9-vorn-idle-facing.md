Vorn_UpdateIdleFacing matches all 356 bytes without its register pin and barrier.
In the final animation transition, the shift produces a facing value and the
quadrant mask is the accumulator for the AND. Reusing the facing as the
accumulator instead swaps the two low registers.
