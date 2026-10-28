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
