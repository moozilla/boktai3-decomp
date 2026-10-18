#include "global.h"
void *Inventory_GetEquipmentEntry(u32);
void Inventory_InitEquipmentEntryFromDescriptor(u8 *, void *);
void Inventory_MarkEquipmentObtained(u32);
void Inventory_SetEquipmentEntry(u32 a, u8 *p)
{
    Inventory_InitEquipmentEntryFromDescriptor(p, Inventory_GetEquipmentEntry(a));
    Inventory_MarkEquipmentObtained(p[0]);
}
