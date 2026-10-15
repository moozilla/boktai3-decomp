# HUD clock, numeric labels and keyframe digits

The reviewed `0805DE3C–0805E190` span contains six functions: five new exact
functions (820 bytes, +0.0339 percentage points), and the existing 32-byte hide
helper. The palette setter's 20-byte extent ends before an independent 136-byte
function at E014. This boundary has its own prologue, four argument handoffs,
pop/bx return and literal pool through E09C. Build/progress extra-boundary
metadata splits the former combined extent without adding bytes to the total.

DE3C writes three map cells beginning at(20,0), then two two-digit numbers at
(24,0)/(27,0). Mode0 uses base hour/minute through the existing clock getters;
mode1 uses hourA/minuteA, other modes hourB/minuteB. These identities follow
sun_clock.h's existing layout, rather than guesses from the glyph labels.
The glyph indices are kept as observed; no translated label is asserted.
The getters now use the observed 32-bit return contract and typed clock fields,
with their existing 12-byte extents unchanged.

DF08 writes five label cells at(20,1), then five decimal places from divisor
10000 at(24,1). Mode0 reads signed game-state halfword7B0; mode1 reads word50C
and uses digit style1; other modes read word77C and use style0. The meanings of
these three numeric values remain unknown. Temporary defined-value register
hints preserve glyph copies and the signed-load register choice. Depin reduced
clock hints 34→15 and numeric hints 58→23; both units carry the concession marker.

E000 stores a narrowed palette bank at widget3A and points widget48 into the
shared 32-byte RGB15 bank array. E014 visits up to three nonnull widgets and
selects bank374 when the owner flag helper accepts masks4/8/16, otherwise373.
The existing helper E7D0 returns owner byte7 masked with the argument; a nonzero
result is enough here. Names describe observed flags and palette selection.

E09C reveals five consecutive 96-byte widgets at parent1308: a label followed
by four digits. Parent resource data starts at20. It clears each hidden flag,
copies the caller's eight-byte position, and loads label keyframeA8+argument
or digit keyframesAA+value, narrowing keyframe IDs to16 bits. Divisions1000,
100 and10 form four digits; no clamping is added. Digit X positions advance
by6, with Y/Z/padding preserved. E170 hides all five widgets. These functions
and both palette functions are ordinary C.

SceneInputWidget is shared with the neighboring input subsection: flags8,
position20, palette bank3A, palette pointer48 and stride60. These added fields
replace opaque padding while preserving all prior consumers' exact extents.
No final screen identity or runtime coverage is claimed.
