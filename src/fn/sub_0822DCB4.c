#define ItemRecord_MeetsLevelRequirement sub_0822DCB4
#define ItemRecord_GetType Inventory_GetEquipmentId
#define Selection_ContainsId Inventory_HasItem
#define gItemDescriptorDefaults          gUnk_08E6092C
#include "game_state.h"
s32 Inventory_HasItem(s32);
s32 Inventory_GetEquipmentId(u32);
struct ItemDescriptorDefaults {
    u8 type, unknown1, requiredLevel, unknown3[29];
};
extern const struct ItemDescriptorDefaults gUnk_08E6092C[];
u32 sub_0822DCB4(u32 record)
{
    if (!Inventory_HasItem(0x29)) {
        s32 level = gUnk_02000710->level;
        const struct ItemDescriptorDefaults *defaults = gUnk_08E6092C;
        if (level < defaults[Inventory_GetEquipmentId(record)].requiredLevel)
            return 0;
    }
    return 1;
}
