#include "scene_item_values.h"
struct E {
    u8 p[6];
    u16 h;
    u8 q[24];
};
u32 Inventory_IsEquipmentAssigned(u32);
extern struct E gUnk_08E6092C[];
u32 Inventory_CanAssignEquipment(u32 slot)
{
    s32 i = Inventory_GetEquipmentId(slot);
    if (i == 0xff)
        return 0;
    if (gUnk_08E6092C[i].h == 0)
        return 0;
    if (Inventory_IsEquipmentAssigned(slot) != 0)
        return 0;
    return 1;
}
