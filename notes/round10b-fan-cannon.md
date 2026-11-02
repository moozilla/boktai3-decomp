# FanCannon_UpdateActive (08209704)

Tags: #matching #fan-cannon #rng #gcse #register-allocation

All 820 bytes match in the normal stock agbcc build, including the literal
pools. The function SHA-256 is
`cc51d5c5997b91229c37ebc45a29b525330357df5a47c671848b344ee3646a00`.

Each directional jitter draw and the later frame draw compute the same
symbol-relative particle sample origin. This reproduces the otherwise dead
0x0403B714 loads and the copies after the reverse-flag reads. The origin uses
the existing random table and the same address delta as the matched particle
routine at 081D0928. Reading the unsigned coordinate before narrowing the
sample to its low byte preserves the order of the halfword and byte loads.

One source pointer is reused for the range samples, the projected offset and
the last pair of particle flag draws. Its allocation leaves the emission
position in r9 across calls while the final vector addition can use r2.
In the reverse branch the range sample is evaluated before the output pointer
is selected for the projection. This places the move after the stack-address
calculation, removing the final instruction-order difference.
