#include "global.h"
s32 Inventory_GetEquipmentId(u32);
struct EquipmentInfo {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 pad[29];
};
extern struct EquipmentInfo gUnk_08E6092C[];

u8 sub_0822D6EC(u32 slot)
{
    return gUnk_08E6092C[Inventory_GetEquipmentId(slot)].unk2;
}
