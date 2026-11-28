#include "global.h"
s32 Inventory_AddEquipmentFromRecord(void *);
u32 Inventory_TryAddEquipmentEntry(void *record)
{
    return Inventory_AddEquipmentFromRecord(record) > 0;
}
