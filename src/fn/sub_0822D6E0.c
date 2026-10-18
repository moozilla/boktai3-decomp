#include "global.h"
u8 *Inventory_GetEquipmentEntry(void);
u8 Inventory_GetEquipmentId(void)
{
    return *Inventory_GetEquipmentEntry();
}
