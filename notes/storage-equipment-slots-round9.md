# Storage_RefreshEquipmentSlots

All 556 bytes match. The carried grid advances its slot index by four per row.
The stored grid derives its row address from the same four-slot stride and
reuses the equipment-ID local from the carried loop. Preserving the stored
loop's back edge keeps agbcc from changing its induction variables.
