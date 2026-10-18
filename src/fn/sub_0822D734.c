#include "global.h"
/* Inventory_GetEquipmentCondition: Returns the equipment entry halfword at offset 2 used as condition */
#define Inventory_GetEquipmentCondition sub_0822D734

u8 *Inventory_GetEquipmentEntry(void);

u16 sub_0822D734(void)
{
    return *(u16 *)(Inventory_GetEquipmentEntry() + 2);
}
