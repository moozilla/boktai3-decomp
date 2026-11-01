# Lun route progress rate

Tags: #monsters #matching

Lun_GetRouteProgressRate matches all 44 bytes without its assembly barrier. The movement
progress is an unsigned halfword. Shifting it by seven gives a phase from 0 to 511.
The table index is a signed halfword assigned from that phase, and the cap tests the
full-width phase. agbcc keeps the copy into the index and needs no sign extension.
