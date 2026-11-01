# Storage_RefreshEquipmentSlots

All 556 bytes match. The carried grid advances its slot index by four per row.
The stored grid derives its row address from the same four-slot stride and
reuses the equipment-ID local from the carried loop. Preserving the stored
loop's back edge keeps agbcc from changing its induction variables.

The upstream match in `d29e12ac5` supplies the loop exits and offset expressions
that let the function assign both empty-slot sentinels directly. This version
uses the inventory declarations in `scene_item_values.h`. See
[the upstream matching note](opus-storage-equipment-slots_2026-11-01.md) for the
loop-reversal diagnostics.
