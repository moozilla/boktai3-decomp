#include "global.h"
#include "scene_inventory_states.h"
struct MenuPanel {
    u8 pad0[0x1754];
    u8 page;
    u8 pageCount;
    u8 pad1756[2];
    u8 pageTypes[0x38];
    u8 entries[1];
};
s32 Inventory_GetItemId(u32);
s32 Inventory_GetEquipmentId(u32);
s32 MenuPanel_IsEntryEmpty(struct MenuPanel *panel, s32 index)
{
    u32 entry = panel->entries[index];
    switch ((s32)(panel->pageTypes[panel->page] >> 1)) {
        case 0:
            if (Inventory_GetItemId(entry) < 0)
                return 1;
            break;
        case 1:
            if (Inventory_GetEquipmentId(entry) == 0xFF)
                return 1;
            break;
        case 2: {
            u32 address = entry;
            struct GameState *game = gGameState;
            address <<= 1;
            address += (u32)game;
            if (((struct SceneInventoryConsumableSlot *)address)->id < 0)
                return 1;
            break;
        }
    }
    return 0;
}
