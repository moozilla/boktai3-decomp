# Terrain edge marker manager (tentative identity)

`0805606C–08056A60` is a coherent manager registered under lookup key `0xFD22`.
`TerrainEdgeMarker` describes the observed behavior; it does not establish a
particular in-game object or execution coverage. The next function at
`08056A60` belongs to script argument dispatch.

The manager allocates `0x2C8` bytes at task priority 9. Its task header occupies
`0x18` bytes, followed by mode at `0x18`, an unknown halfword at `0x1A`, count at
`0x1C`, and blink frame at `0x1E`. Seventeen `0x28` sprite records begin at
`0x20`. A sprite's render flags are at zero, render-mode byte at `0x0F`, and
8-byte world position at `0x18`. `include/terrain_edge_marker.h` gives the
shared view. Position words preserve the unknown final halfword when script
coordinates do not initialize it.

Insertion uses animation resource `0x1C1A`, frame `0x33` for kind zero or
`kind + 0x34` otherwise, and scale `(-8, -8)`. Direction 0 adds no flip bits;
1, 2, and the remaining nonzero values add flags `4`, `12`, and `8` respectively.
Render mode becomes 1. Unless the preserve-height argument is nonzero, Y is
replaced with raw terrain height times 256 plus 128. X and Z are signed 8.8
coordinates for terrain indexing; an out-of-bounds coordinate selects index
zero. The insertion function appends without checking the seventeen-entry
capacity. The null-cell pointer path remains as observed in the original.

The direct script wrapper reads `p` (three coordinates, default zero), `f`
(preserve height, default zero), `d` (direction input, default zero), and `t`
(kind, default zero). Its direction conversion is `((d >> 1) + 1) & 3`.
The mode wrapper reads `m`; mode 1 immediately hides every marker and clears
the blink frame. Initialization clears mode/count/blink frame and registers
the manager. Update and destruction callbacks return zero; creation installs
them through the task API's callback view and preserves its initialization
failure branch.

Visibility is enabled only when mode is zero and the signed halfword at
`gUnk_02000710 + 0x14` is nonzero. The old frame values 0–47 show the markers;
48–71 hide them. The counter increments and resets after 71. Otherwise it is
cleared and all markers are hidden. Showing clears sprite flag 1; hiding sets
it. Destruction removes each counted sprite.

The region consumer reads `n` (default zero), narrows it to a 16-bit lookup
tag, and calls `08021E104` for a region and count. Region records are `0x0C`
bytes: signed min X/Z and max X/Z at `0/2/4/6`, an unknown halfword at 8, and
tag at `0x0A`. The consumer computes signed midpoint coordinates and samples
a terrain override when present, otherwise the raw cell. The lower nibble
is height; upper-nibble kinds 1 and 2 subtract the low byte of Z or X from Y.
It then asks the edge selector for a direction and, if successful, reads `t`
and inserts with preserve-height zero. Thus insertion recomputes Y from the
raw terrain height plus 128 even though the consumer already calculated a
slope-adjusted Y. This apparent redundancy is preserved.

The pure selector probes sides in order: positive X (return 0), positive Z
(1), negative X (2), negative Z (3), otherwise -1. For each side it probes
sixteen world units inside and outside the edge, with the other coordinate
at the signed midpoint. If the indices are equal it accepts immediately.
For different indices, it requires the outside cell's flag bit 0 to be clear
and the following raw-terrain connection:

| Side | Inside kind | Outside kind | Height condition |
| --- | --- | --- | --- |
| +X | 0 | 0 or 2 | inside = outside |
| +X | 2 | 0 or 2 | inside = outside + 1 |
| +Z | 0 | 0 or 1 | inside = outside |
| +Z | 1 | 0 or 1 | inside = outside + 1 |
| -X | 0 or 2 | 0 | inside = outside |
| -X | 0 or 2 | 2 | inside + 1 = outside |
| -Z | 0 or 1 | 0 | inside = outside |
| -Z | 0 or 1 | 1 | inside + 1 = outside |

These heights are the lower nibbles of the raw cell bytes. The selector does
not consult terrain overrides. Its out-of-bounds probes also map to index
zero. Empty barriers and selected register bindings are temporary matching
concessions; no explicit r7 binding or handwritten instruction body is used.

The reviewed cohort contains five newly recovered functions totaling 2,236
bytes, a 0.0926 percentage-point gain, plus four already-C functions totaling
312 bytes whose contracts now share the family header. Each translation unit
was checked over its complete reviewed extent with the fast checker. Combined
ROM validation belongs to the integrator's batch.
