#include "global.h"
void ItemDescriptor_Reset(void *);
void *Inventory_GetEquipmentEntry(u32);
void Inventory_InitEquipmentEntryFromDescriptor(void *, void *);
void Inventory_ClearEquipmentEntry(u32 a)
{
    u8 b[0x20];
    ItemDescriptor_Reset(b);
    Inventory_InitEquipmentEntryFromDescriptor(b, Inventory_GetEquipmentEntry(a));
}
