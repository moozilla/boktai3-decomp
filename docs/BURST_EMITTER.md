# Five-slot burst emitter (tentative identity)

Reviewed range `08069964–08069FA4` is a complete five-slot manager at
`02000160`. The task occupies `0x224` bytes: used mask `0x18`, five entries
at `0x1C`, stride `0x68`. Every reviewed extent, including the pre-existing
four-byte idle callback `69C94`, is exact. Nine new functions add 1,492 bytes
(about 0.0618 percentage points). Existing 108 bytes receive contracts only.

Each entry has a contact collider at zero, position `0x54`, frame `0x5C`,
mode `0x60`, state `0x61`, signed slot index `0x62`, and context word `0x64`.
The context's identity is unconfirmed. State 0 is idle, 1 emits/advances,
and 2 releases. The release clears the entire entry after clearing its mask
bit; the intermediate index `-1` does not survive that clear.

The active callback emits during frames 0–5, refreshes the collider during
frames 10–15, and advances to release after frame 40. The frame-4 emission
consumes two RNG samples per particle and publishes only the second index.
Ambient sound is suppressed by engine flag `0x800` or nonzero context, and
otherwise checks a random 9–12 frame divisor before playing song `0x339`.
The collider uses size `(256,256,256)`, mask `0x2001`, tag `0x3246`, and a
context-dependent contact mask. Packed vector writes preserve unused padding
bits through compiler-generated masks; no authored scalar padding read is
used to supply a position.

`69D50` takes a required position pointer plus mode/context and four full-word
collider arguments, truncates the final collider argument to 16 bits, and
always returns zero. `69DB8` reads optional `p` coordinates, optional `P`
four signed 16-bit collider arguments, and `c` context default zero. Missing
`p` preserves the already-cleared/reused slot position. This differs from
`697C8`, whose absent-`p` path passes an uninitialized stack vector: that
wrapper remains an ignored draft pending actual script caller evidence.

The allocator (68 bytes) retains three minimized allocation hints; frame
emission (516 bytes) retains five. Other new bodies are ordinary C. No
explicit r7 binding or instruction body is used. All hints were minimized
with `depin.py --write`; the integrator must run the combined full ROM build.
