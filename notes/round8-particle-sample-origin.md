# Particle sample address origin

Tags: #rng #matching #round8

The word `0403B714` occurs at 29 aligned locations in the stock ROM.
Twenty-eight are Thumb literal-pool words. Each has one PC-relative load.
The remaining word at `0824E1A4` lies between copies of the
`../system/eneexin.h` filename in read-only data. No aligned ROM pointer
targets that word.

Following the loaded registers through moves, branches and calls found no
memory access, arithmetic operation or comparison that consumes the value.
Some callers copy it once or twice before overwriting it. The value can reach
a call to `Player_SetDirectionalVelocity` in r3, but that function's player
and speed arguments occupy r0 and r1.

| FanCannon_UpdateActive | First direction | Second direction |
|---|---|---|
| Load into r3 | 08209744 | 08209798 |
| Copy r3 into r6 | 08209772 | 082097C8 |
| Literal-pool word | 08209780 | 08209800 |

The observed value can be expressed as a named numeric origin for the frame
sample. Adding the cursor's byte offset to the origin and then subtracting
the origin before indexing the physical sample table reproduces both literal
loads with stock agbcc. That candidate places the loads at the later capture
of the sample cursor. The original code places them before the first draw in
each direction. The origin's meaning before compilation is still unknown.

ActorParticles_SpawnBurst at 0812E5A4 reproduces all 332 bytes with a named
statement macro that captures the numeric origin. The first draw uses the
physical table address. The three spread draws and the count draw use
ParticleRandom_ReadRelativeSample. This removes both assembly barriers and
retains the original early load without using addresses from other RAM objects.
