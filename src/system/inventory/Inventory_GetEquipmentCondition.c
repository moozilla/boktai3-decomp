#include "global.h"
/* Inventory_GetEquipmentCondition: Returns the equipment entry halfword at offset 2 used as condition */

u8 *Inventory_GetEquipmentEntry(u32);

u16 Inventory_GetEquipmentCondition(u32 slot)
{
    return *(u16 *)(Inventory_GetEquipmentEntry(slot) + 2);
}
