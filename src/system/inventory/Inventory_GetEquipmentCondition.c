#include "ui/town/shop_item_values.h"

/* Inventory_GetEquipmentCondition: Returns the condition of an equipment entry */
u16 Inventory_GetEquipmentCondition(u32 slot)
{
    return Inventory_GetEquipmentEntry(slot)->condition;
}
