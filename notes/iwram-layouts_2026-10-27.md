# Shared IWRAM object layouts (2026-10-27)

Tags: #matching #tooling #ram

## 030042A0: render camera

`gCamera` in `include/render.h` is a 0x14-byte `CameraRenderState`, distinct
from the active camera task pointer. The merged declarations and ROM agree on
signed position X/Y/Z, screen offsets at +08/+0A, and projection mode at +12.
The position tail at +06 is copied; its independent meaning is unresolved.
+0C and +10 are initialized to 1 and 0; +0E remains padding. Semantic names
are provisional. Forging-book views called +04 Y; they now select position Z
and retain their unsigned reads. Whole-position copies preserve eight bytes.

All 30 direct C consumers pass `tools/check.py` against the ROM. Before/after
agbcc assembly is identical (allowing the address-symbol rename) in default
and NONMATCHING modes for 283 direct and header-dependent units. No protected
source file changed. No full build was run; batch integration owns that check.

Tool gap: a struct migration mode with per-file field mappings and a protected
file exclusion list would avoid manual baseline collection and repeated checks.

## 03004C30 / 03004CC0: background records

`gBgRecords[4]` shares one 0x30-byte `BackgroundLayer` in `include/render.h`.
The storage map agrees with the ROM and the outside audit: +00/+02 carry
resource mode/layer, +10/+12 scale or overlay tile coordinates, +14/+16 origin,
+18/+1A dimensions, +1C/+1E copied dimensions, +20/+22 scroll, and +24/+28/+2C
resource, map-data and output pointers. Names remain provisional where modes
reuse fields. Signed dimension/scroll fields follow signed ROM readers;
unsigned uploader reads retain explicit casts. The initializer's pointer words
now have pointer types. Existing pause-menu aggregate copies still match.

03004CC0 is record 3, and its consumers now select `gBgRecords[3]` rather than
declaring another object. The scene overlay retains its literal-address reload
constraints through the common type. `Bg_UpdateScrollShadow` retains raw +B0
and +B2 reads: spelling those as fields produces 172 bytes versus the ROM's
180 by sharing a record base, as independently witnessed in the research kit.

All 28 changed direct C consumers pass per-unit ROM checks. The protected
`scanline_presentation.c` and `sub_08110C9C.c` remain untouched with their old
address declarations: their default builds match and their draft mismatches
are unchanged. Before/after assembly agrees in both modes for all 265 units
reached by the original declarations and headers. A protected-file exclusion
and field-remapping support in ramlayout would make this migration faster.
