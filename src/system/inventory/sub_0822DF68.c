#include "global.h"
s32 Inventory_GetEquipmentId(u32);
bool32 Inventory_IsEquipmentAssigned(s32);
struct EquipmentInfo {
    u8 unk0[6];
    u16 unk6;
    u8 pad[24];
};
extern struct EquipmentInfo gUnk_08E6092C[];

s32 sub_0822DF68(void)
{
    s32 count = 0;
    s32 i;
    for (i = 0; i <= 15; i++) {
        if (gUnk_08E6092C[Inventory_GetEquipmentId(i)].unk6 != 0 &&
            !Inventory_IsEquipmentAssigned(i))
            count++;
    }
    return count;
}
