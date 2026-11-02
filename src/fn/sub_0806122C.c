#include "scene_inventory_control.h"
s32 SceneInventory_ClassifyConsumableAction(struct SceneInventoryControlView *panel)
{
    s32 result;
    s32 item;
    if (panel->grid.selection > 15) {
        s32 pageIndex = (panel->page << 3) - 16;
        item =
            ((struct SceneInventoryCatalogView *)panel)->entries[pageIndex + panel->grid.selection];
        result = 1;
        if (item < 0)
            result = 0;
        else if (gGameState->unk7B0 < SceneItemValue_GetConsumableBaseValue(item))
            result = 0;
        else if (panel->destination < 0)
            result = 0;
    } else {
        item = ((struct GameState *)((u8 *)gGameState + panel->grid.selection * 2))->table160[0];
        if (GameState_IsConsumableSlotEquipped(panel->grid.selection))
            result = 0;
        else if (item < 0)
            result = 0;
        else {
            s32 value = GameState_GetConsumableDescriptor(item)->baseValue;
            result = -1;
            if (!value)
                result = 0;
        }
    }
    return result;
}
