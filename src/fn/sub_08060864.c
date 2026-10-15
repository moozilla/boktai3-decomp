// MATCHING_CONCESSION: register-pinning
#include "scene_inventory_actions.h"
#define SceneInventory_ClassifyEquipmentMove sub_08060864
struct SceneInventoryRestoreView {
    u8 unknown0[0x1AEC];
    u16 fallbackSelection;
    u8 unknown1AEE[0x1BE5 - 0x1AEE];
    u8 cursorX, cursorY, selection;
};
s32 SceneInventory_ClassifyEquipmentMove(struct SceneInventoryActionView *panel)
{
    u32 saved = panel->savedSelection;
    register u32 current asm("r5");
    if (saved <= 15) {
        s32 oldKind, newKind;
        current = panel->selection;
        if (saved == current)
            return 0;
        oldKind = sub_0822D6E0(saved);
        newKind = sub_0822D6E0(current);
        if (oldKind == 255 && newKind == 255)
            return 0;
        if (panel->valueMode == 1)
            return 1;
    } else {
        struct SceneInventoryRestoreView *restore = (void *)panel;
        register u8 *selection asm("r2") = &panel->selection;
        u32 selected;
        selected = *selection;
        if (selected == saved) {
            u32 fallback = restore->fallbackSelection;
            u32 column;
            *selection = fallback;
            column = 3;
            column &= fallback;
            restore->cursorX = column + 4;
            restore->cursorY = *selection >> 2;
            return 4;
        }
        if (selected <= 15) {
            register u32 reread asm("r5") = *(volatile u8 *)selection;
            if (sub_0822D6E0(reread) == 255)
                return 4;
        }
    }
    return 0;
}
