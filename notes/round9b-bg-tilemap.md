# Background tilemap window

Tags: #matching #background #tilemap

BgGfx_LoadTilemap at 08216584 expands each metatile into a pair of tilemap rows. Large scroll changes redraw the full 32-column window. Small changes update newly exposed strips. The clipped path draws the new column first, then the row. The wrapping and interior paths draw the rows first.

The 2,248-byte guarded draft compares at 87.7% against the 2,256-byte ROM function. Its stack frame is 0x48 bytes. The ROM has an additional word at 0x48 used for column parity and the next column index. Indexing each metatile as four halfwords recovers the order of its tile reads.

In 2,160 paired Thumb executions, the complete 0x30-byte layer record and the destination window plus its guard agreed. The comparison varied mode, map dimensions, source override, origin and scroll delta. It included zero movement, clipped edges, wraparound and both single-strip and full redraws.
