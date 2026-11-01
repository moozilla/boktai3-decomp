# Background tilemap window

Tags: #matching #background #tilemap

BgGfx_LoadTilemap at 08216584 matches all 2,256 bytes with stock agbcc. It expands each metatile into a pair of tilemap rows. Large scroll changes redraw the full 32-column window. Small changes update newly exposed strips. The clipped path draws the new column first, then the row. The wrapping and interior paths draw the rows first.

The loops use one block index throughout the function. The clipped row and column also share their column coordinate. This gives the column parity and next-column index the same spill word in the 0x4C-byte frame. During the clipped row update, the saved horizontal scroll argument can hold the exposed row coordinate.

In 2,160 paired Thumb executions, the complete 0x30-byte layer record and the destination window plus its guard agreed. The comparison varied mode, map dimensions, source override, origin and scroll delta. It included zero movement, clipped edges, wraparound and both single-strip and full redraws.
