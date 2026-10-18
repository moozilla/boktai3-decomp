#define MenuPanel_FindEquipmentSlot Storage_FindEquippedItemSlot
#include "global.h"
struct MenuPanel {
    u8 pad0[0x1754];
    u8 page;
    u8 pageCount;
    u8 pad1756[2];
    u8 pageTypes[0x38];
    u8 entries[1];
};
s32 Storage_FindEquippedItemSlot(struct MenuPanel *, s32);
s32 GameState_IsConsumableSlotEquipped(u32);
s32 MenuPanel_IsEntryAvailable(struct MenuPanel *panel, s32 index)
{
    switch ((s32)(panel->pageTypes[panel->page] >> 1)) {
        case 0:
        default:
            break;
        case 1:
            if (Storage_FindEquippedItemSlot(panel, panel->entries[index]) >= 0)
                return 1;
            break;
        case 2:
            if (GameState_IsConsumableSlotEquipped(panel->entries[index]))
                return 1;
            break;
    }
    return 0;
}
