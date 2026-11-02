#include "scene_inventory_actions.h"
struct SceneInventoryRestoreView {
    u8 unknown0[0x1AEC];
    u16 fallbackSelection;
    u8 unknown1AEE[0x1BE5 - 0x1AEE];
    u8 cursorX, cursorY, selection;
};
s32 SceneInventory_ClassifyEquipmentMove(struct SceneInventoryActionView *panel)
{
    u8 saved = panel->savedSelection;
    u32 current;
    if (saved <= 15) {
        s32 oldKind, newKind;
        current = panel->selection;
        if (saved == current)
            return 0;
        oldKind = Inventory_GetEquipmentId(saved);
        newKind = Inventory_GetEquipmentId(current);
        if (oldKind == 255 && newKind == 255)
            return 0;
        if (panel->valueMode == 1)
            return 1;
    } else {
        struct SceneInventoryRestoreView *restore = (void *)panel;
        if (panel->selection == saved) {
            u32 fallback = restore->fallbackSelection;
            u32 column;
            panel->selection = fallback;
            column = 3;
            column &= fallback;
            restore->cursorX = column + 4;
            restore->cursorY = panel->selection >> 2;
            return 4;
        }
        if (panel->selection <= 15) {
            current = panel->selection;
            if (Inventory_GetEquipmentId(current) == 255)
                return 4;
        }
    }
    return 0;
}
