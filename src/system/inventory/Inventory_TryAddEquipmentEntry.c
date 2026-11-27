#include "global.h"
s32 Inventory_AddEquipmentFromRecord(void *);
u32 Inventory_TryAddEquipmentEntry(void *record)
{
    s32 v = Inventory_AddEquipmentFromRecord(record);
    u32 r = 0;
    if (v > 0)
        r = 1;
    return r;
}
