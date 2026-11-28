#include "system/game_state/game_state.h"
s32 Inventory_HasItem(s32);
s32 Inventory_GetEquipmentId(u32);
struct ItemDescriptorDefaults {
    u8 type, unknown1, requiredLevel, unknown3[29];
};
extern const struct ItemDescriptorDefaults gUnk_08E6092C[];
u32 ItemRecord_MeetsLevelRequirement(u32 record)
{
    if (!Inventory_HasItem(0x29)) {
        if (gGameState->level < gUnk_08E6092C[Inventory_GetEquipmentId(record)].requiredLevel)
            return 0;
    }
    return 1;
}
