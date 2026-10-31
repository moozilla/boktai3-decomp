# FeatherEffectPool_SpawnGroundScatter

All 828 bytes at 08128F98 match with stock agbcc. Assigning the initial cell
index directly to its retained local introduces the r0 reload on the
out-of-bounds path. The next reload then loads the random-index address into
r1 at the loop head.

For the Z displacement, the 32-bit ring mask and the 16-bit lookup result
occupy one operand in succession. The halfword load then uses r1 for its
address and destination, leaving r0 for the low-byte mask.
