# Hit-counted switch pool (tentative identity)

`08054294–08054B80` is a coherent pool owned by `gUnk_02000490`: allocation
`0x8FC`, priority 8, eight `0x118` entries beginning at `0x3C`. `HitSwitch` is a
provisional descriptive name. These are static contracts, without execution
coverage or proof of a particular in-game switch identity.

The common layout is in `include/hit_switch.h`. The manager stores count at
`0x18`, pending-hit latch at `0x19`, flash counter at `0x1B`, and sixteen RGB15
colors at `0x1C`. Each entry has a sprite at zero, graphics at `0x2C`, collider
at `0x48`, body at `0x9C`, world position at `0xE4`, kind at `0xEC`, optional-body
flag at `0xED`, collision ID at `0xEE`, signed query/script tag at `0xF0`, terrain
cell index at `0xF2`, frame and bob period at `0xF4/0xF6`, hit/required counts at
`0xF8/0xFA`, cooldown at `0xFC`, completion flag at `0xFE`, cell-registration
flag at `0xFF`, cell reference at `0x100`, callback at `0x110`, and completion
script resource ID at `0x114`.

The collider callback tests the other collider's `contactFlags2` at `+0x38`,
mask `0x20`. With zero cooldown it increments hit count, sets a 32-tick cooldown,
and latches a shared hit. When enough hits arrive, both idle variants mark
completion, register a terrain cell, disable collision via bit 4, and enter an
eight-tick completion animation. Sound 426 plays on its first tick; frame 1 is
shown until tick 8, then frame 2, and a nonzero script ID is called with one
signed argument: the entry tag. State callbacks return void and reset their
frame via the shared setter. Manager update/destruction return zero.

Kind zero is static. Its completed cell kind is 1 or 2 according to sprite
flag 4, and its height is truncated world Y/256 plus one. Other kinds use cell
kind zero and truncated world Y/256. The bobbing idle variant has amplitude
12 from `gUnk_086149C4`, period from the script, and signed truncation toward
zero after scaling. Once complete, it moves down by two toward world Y minus
16 while `gUnk_02000580+0x28A` equals its terrain index, playing sound 691 on
the first downward step; otherwise it moves back toward world Y.

The manager consumes the shared hit latch with sound 425 and resets flash to
32. It decrements flash, copies palette 214 for the upper half, then blends
sixteen colors from palette 214 toward palette 213 over the lower sixteen
ticks, and copies palette 213 at zero. Each RGB channel is an independent
5-bit weighted sum divided by sixteen. The entry update follows the shared
flash update, dispatches the state, then decrements hit cooldown. During
cooldown it uses palette 214 and the shared palette buffer; on expiry it
selects palette 213. Destruction removes each optional body, sprite, collider,
and registered terrain-cell reference before clearing the singleton.

The script initializer appends at the current count. Keywords: `n` collision
ID (default 0), `p` XYZ position (default zero), `t` kind (default 0), `r` sprite
flag 4, `h` optional body with helper kind 7, `A` required hits (default 10), `W`
bob period (default 0), `s` initial completion (default 0), and `g` signed tag
plus completion script ID (default tag -1 / ID zero). Kind zero loads graphics
`0xC3C3`; other kinds load `0xD166`. Terrain index uses signed X/Z shifted by
eight and scene row offsets, with out-of-range index zero. The body and collider
share the stored world position. Initial completed entries register their cell
immediately and select completed callbacks. The completion query reads keyword
`i` (default 0), returns the first matching tag's completion byte, or zero.

The initializer returns void: its final epilogue overwrites r0 with its return
address. `HitSwitch_AddFromScript` is registry key `0x1E98` and is called through
the generic registry dispatcher `08225590`, which stores returned r0 into script
state `02000610+4`. The wrapper preserves that incidental r0 via one empty output
barrier; no semantic return value is asserted. This avoids an incompatible call
prototype or a fabricated count return. Creation caches the singleton, creates
when absent, and passes the returned manager directly to the initializer.

All 17 functions match their full reviewed extents: ten newly exact functions,
1,928 bytes / +0.0798 percentage points of the `0x24DC0C` denominator. New bytes
are 852 unpinned and 1,076 with minimized concessions (completion animation,
palette blend, initializer). Seven preexisting helpers total 356 bytes and get
no new byte credit; the 28-byte wrapper now retains the ABI concession above.
The worker ran fast object checks after formatting. The integrator runs the
combined ROM build before merging the batch.
