#include "scene_inventory_tail.h"
s32 SceneInventory_TickActionSelection(struct SceneInventoryTail *p)
{
    SceneInputPanel_HighlightCompare(&p->display, p->highlight.colors);
    if (SceneInventory_HandleClockShortcut(p))
        return 0;
    if (SceneInventory_HandlePageKeys(p)) {
        SceneInventory_UpdateModePrompt(p, 1);
        return 0;
    }
    if (gPadState[1] & 2) {
        Sound_PlaySong(0xDE);
        SceneInventory_BeginBrowse(p);
    } else if (1 & gPadState[1]) {
        switch (p->category) {
            case 0:
                SceneInventory_ConfirmEquipmentAction(p);
                break;
            case 1:
                SceneInventory_ConfirmItemAction(p);
                break;
            case 2:
                SceneInventory_ConfirmConsumableAction(p);
                break;
        }
    } else if (SceneInput_NavigateGrid(&p->grid)) {
        p->interactionFlags |= 0x80;
        SceneInventory_UpdateModePrompt(p, 1);
    }
    return 0;
}
