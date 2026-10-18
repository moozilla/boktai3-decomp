#include "global.h"
struct E {
    u8 p[6];
    u16 h;
    u8 q[24];
};
u32 Inventory_GetEquipmentId(void);
u32 Inventory_IsEquipmentAssigned(u32);
extern struct E gUnk_08E6092C[];
u32 Inventory_CanAssignEquipment(u32 a)
{
    s32 i = Inventory_GetEquipmentId();
    if (i == 0xff)
        return 0;
    if (gUnk_08E6092C[i].h == 0)
        return 0;
    if (Inventory_IsEquipmentAssigned(a) != 0)
        return 0;
    return 1;
}
