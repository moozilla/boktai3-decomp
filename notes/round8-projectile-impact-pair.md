# Projectile_SpawnImpactPair

Tags: #matching #effects #compiler #round8

All 476 bytes at 081D9D3C match with stock agbcc. The RNG setup captures the
cursor, mask and table before the two children spawn. The existing particle
sample-read macro gives the table a reference in each of the five draws.
Explicit sector and jitter masks keep the value 63 from occupying the first
heading's register. Capturing the output parameter record preserves the flag
load and OR at the start of each iteration.
