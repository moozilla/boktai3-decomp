# TilemapPanel_StreamColumns

All 376 bytes match. The recorded scroll edge identifies the 8-pixel columns
that have just entered the screen. A two-column update can straddle a 32-pixel
tile boundary. Each switch arm places its ending pixel edge in the same cursor
after the final draw, so agbcc shares that call across all four cases.
