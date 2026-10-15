# Stored-resource station (Sol, 2026-10-14)

Adds **0.0346 percentage points** exact code (836 bytes, three functions),
with188 unpinned bytes and648 register-pinned bytes after depin. Cumulative
worker gain is **0.2118 percentage points /5,116 bytes /27 new functions**.
The three complete reviewed extents pass fast checks. The integrator owns the
combined ROM build. No emulator claims.

**0804EFB4–0804F6C4** is fully exact (1,808 contiguous bytes, including existing
helpers). `ResourceStation` is tentative: the object transfers GameState+77C
stored resource into player+428, capped against+42A. The exact gameplay label
for that resource remains unconfirmed. Its creator allocates0x268 bytes, matching
`include/resource_station.h`.

Layout: task18 bytes, sprite18 (position34), sprite graphics44, digit resource60,
four0x60-byte digit animations80, collider200, trigger position254, player25C,
script handle260, state262, animation frame263, nearby264, digit visibility265,
interaction variant266. Digit flags lie8 bytes into each animation and position
at20. The player view records observed input pointer4EC, busy byte464,
character kind418 and action457.

New EFB4(256) accepts player positions on exactly the trigger's Y plane and
within128 units on each X/Z axis. Only player kind1 can be nearby. Interaction
is suppressed by globals02000534/nonzero plus02000500/value5, character kinds
3/4/5, busy464, missing input flag1, or action other than7. Otherwise it invokes
08158ECC(player,trigger,variant) and enters station state1. Observed player
pointer reloads are retained.

New F140(188) drives sprite graphics1AE/1AF/1B0 from player flag40 and an eight-
step repeating phase. Once that flag clears, animationFrame is clamped to4,
counts down with tile1AF and settles at1AE.

New F1FC(392) displays four decimal digits when the combined halfword at264 is
nonzero (either proximity or script-controlled digit visibility). GameState+77C
is capped at9999, divided into thousands/hundreds/tens/ones, and mapped to digit
keyframes30..39. Projection uses signed truncation toward zero of3/32 times
X-Z, X+Z and Y, then camera03005418 offsets, centering and eight-pixel spacing.
The resulting screen position and padding are copied to each digit animation.
Flag value1 controls digit visibility.

Existing F0B4 transfers one stored unit per tick only during player action8,
flag40, positive stock and current resource below maximum. Existing F384 reads
script-event values0/1 to show/hide digits. Initialization uses sprite resource
A47C, digit resourceCB05/530D, trigger offset256 along X or Z according to sprite
flag4, and interaction variant7 or1. Cleanup removes the sprite, four animations
and collider. Those helpers were named and verified over their full extents.

Creator F67C is **72 bytes**: a separately authored pre-existing hidden helper
atF6C4 clears the next manager02000110. Do not use the ordinary next seedF6D0
as the creator's extent. Continue with the0xFC-byte entries of that next manager.
