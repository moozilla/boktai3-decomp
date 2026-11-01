#include "scene_item_values.h"

u32 Inventory_GetEquipmentId(u32 slot)
{
    return Inventory_GetEquipmentEntry(slot)->kind;
}
