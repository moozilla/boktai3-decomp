# Link variant of area effects (Sol, 2026-10-14)

Adds **0.0227 percentage points** exact code (548 bytes, four functions),
with60 unpinned bytes and488 register-pinned bytes after depin. Cumulative
worker gain is **0.1772 percentage points / 4,280 bytes /24 new functions**.
All four reviewed extents pass fast checks. Integration owns combined build.

**0804EBD8–0804EFB4** is now fully exact (988 contiguous bytes, including prior
matches). It shares the area manager and entry schema. `area_effect_link.h`
records the observed session view behind020004B4: local player index is a signed
halfword at18, per-player value bytes at5C..5F, shared scalar at62. Their exact
gameplay meanings remain tentative.

New EBD8(40) scales a scalar gain by shared session value /140. The existing
EC00 helper's second argument was incorrectly declared a pointer; it is now a
scalar gain and passes to EBD8. Its reviewed extent is96 (EC60 is another
pre-existing function), not the next named seedECD0.

New ECD0(252) visits four player pointers at02000580. It updates only nonnull
state1 players. Mode2 derives from flag20; otherwise flag10 selects mode1/0.
Kind6 adds40 fractional units for mode2 or5 for mode0. Other kinds add the
session's player value times8 for mode2 or its base value for mode1. Each
player has a separate trailing manager counter atAAC+2*i. Every120 units is
converted to one player resource increment through081594F4. Unlike the solo
callback there is no idle-time gate and no kind2 quadrupling.

New EDD0(196) uses the local session value to select visibility. A nonzero value
shows enabled sprite entries by clearing flag value1. A zero value hides all
entries with sprites by setting that flag. Existing EE94 dispatches this unless
GameState flags868 mask2 selects an empty placeholder, then always runs ECD0.
New EED0(60) removes sprites and clears manager0200010C, matching the solo
cleanup's contract. Existing link initializer/creator and singleton selector
were named and checked exactly.

No gaps or drafts remain in the link variant. Next: EFB4–F6D0, a separate
scripted player-interaction/display object; keep its identity provisional.
