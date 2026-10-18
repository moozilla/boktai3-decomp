#include "global.h"
void sub_0822D7C0(void *);
void *Inventory_GetEquipmentEntry(u32);
void Inventory_InitEquipmentEntryFromDescriptor(void *, void *);
void Inventory_ClearEquipmentEntry(u32 a)
{
    u8 b[0x20];
    sub_0822D7C0(b);
    Inventory_InitEquipmentEntryFromDescriptor(b, Inventory_GetEquipmentEntry(a));
}
