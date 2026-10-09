# Main-thread batch 6 evidence

The three solar sensor routines at 08243C10/08243CDC/08243D28 add 512 exact
native bytes. Timer shutdown and ISR matched on first checks. Initialization
required a natural inline u16 return for the IE mask; direct casting, u32 and
s32 saved-IE variants omitted the original two normalization shifts. Full ROM
build passed before individual commits. All eight RAM snapshots and four
result-store probe rows agree between original and rebuilt ROMs. The four
emulator input thresholds and one-count timing variation are documented in
`docs/SOLAR_SENSOR.md`; the screenshot confirms the late-game outdoor scene.

`082439EC` adds 100 bytes on the first independent C check: seven cleanup calls,
clearing a pointer table entry indexed by object+0x18, decrementing a halfword
count and returning zero. There is insufficient evidence here to name the
specific gameplay object type. Full build passed before its commit.

`08243B54` remains untracked WIP. Its apparent ten-cell tile bar uses integer
scaling `(current-1)*79/(maximum-1)+1`, two full/empty tile choices and a partial
cell. The first 192-byte draft exactly reproduces the prologue, division and
output lookup but differs in mask construction and subtraction scheduling.
Signed tile variables, signed output pointer, a bitfield output experiment,
inline pack helpers and signed-mask lifetimes were tried; none matched. Do not
infer an exclusive solar/UI owner merely from adjacency. Original simple C and
all comparisons remain under ignored `wip/` and `build/endurance/root/`.

The short Sol allocator match at 081B6180 exposed a separate retained 100-byte
Thumb function at 081B61E0–081B6244. Its own push {r4,r5,r6,lr}, argument checks,
call, conditional stores and pop/bx return follow the allocator wrapper's return.
It remains assembly and gets a reviewed unmatched progress boundary, so the
new wrapper contributes only its 96 emitted bytes. No shared gen/ edits or
speculative semantic name were made. The denominator gains one function.
