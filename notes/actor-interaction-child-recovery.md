# Actor_ConfigureInteractionChildFromScript

The script command at `081E9E44` claims the first of sixteen child slots with
control bit 1 set. Each slot is `0xDC` bytes. Its world position begins at
`+0x48` and its callback at `+0xAC`. The scan advances the region list, position,
orientation, sprite and control cursors together. The script fields and final
sprite flip use the selected slot's byte offset from the first slot.

The `t` option selects the callback and effect. The command reads a terrain box
or explicit coordinates, positions the sprite, fills its region list and reads
the optional completion and start scripts. The controller halfword at `+0xF62`
receives -64 or 64 according to the selected orientation.

The stock compiler matches all 892 bytes. The entire controller translation
unit also matches all 30,356 bytes at `081E34D0`.

Tags: 081E9E44, actor animation controller, script, interaction child, agbcc
