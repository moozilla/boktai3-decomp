# Background controllers and scheduler report groups

The first game code after `AgbMain` controls backgrounds rather than startup.
The contiguous `080015AC–08002AA8` range contains two related task families:
normal backgrounds and a scrolling variant. These are provisional names.
This group is now **100% matched**. The completion session raised whole-game
coverage from **15.315% to 15.490% (+0.176 percentage points)** by finishing
this group and the following palette controllers (08002AA8–080036F8), also
now **100% matched**.
No affine-transform role is claimed merely from fixed-point arithmetic.

Both families have allocation, initialization, update, cleanup, script creation,
and a queue of up to eight commands. Script options select resources, positions,
scroll speeds, dimensions, layer modes and blend levels. Updates use camera
position and write background-layer state; the scrolling variant also has a
sine-table-driven movement mode.

New matching C covers both initializers, both queued-command processors, both
script creation commands, map-ID lookup, and the scrolling family's command
submission/queue. The existing allocators and cleanup helpers already match.
`include/background_controller.h` and `include/scrolling_background.h` record
observed layouts, retaining explicitly unknown fields. Local register hints in
the normal initializer and map-ID lookup are temporary matching concessions.

Both frame updates, all map-loading helpers, and both command queues are now
exact C. A separate map-loading wrapper at 080020BC is now counted independently.
The palette family handles color-table blending, brightness scaling, oscillation,
script-driven transitions, resource replacement and hardware palette uploads;
`include/palette_controller.h` records its shared layout. Some register hints use
empty compiler constraints; these emit no handwritten instructions.

## Scheduler findings

The scheduler itself (`08219F50–0821A184`) was already entirely matched before
this pass. It has fourteen lists, a per-list pause mask, update/destructor
callbacks, deferred destruction, nonzero task IDs and ID lookup. Its initialization,
registration, frame loop and destruction are now grouped and named in the report.
The following resource registry and double-buffered message queue are separate
groups. The preceding heap/memory group is now fully matched after adding the
512-byte-chunk DMA helper. Input polling remains a neighboring unfinished helper.

## Reading the start of the map

`AgbMain` at `0800145C` already matches. The earliest report spans contain the
GBA header/reset vector, ARM initialization and the interrupt/IWRAM code blob.
They are shown separately from the C entry point and remain assembly. Grouping
does not recolor those bytes as decompiled or change the historical denominator.
