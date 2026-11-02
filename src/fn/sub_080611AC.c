#include "scene_inventory_control.h"
s32 SceneInventory_ClassifyItemAction(struct SceneInventoryControlView *panel)
{
    s32 result;
    s32 item;
    if (panel->grid.selection > 15) {
        s32 pageIndex = (panel->page << 3) - 16;
        item =
            ((struct SceneInventoryCatalogView *)panel)->entries[pageIndex + panel->grid.selection];
        result = 1;
        if (item < 0 || gGameState->unk7B0 < (s32)SceneItemValue_GetItemBaseValue(item) ||
            panel->destination < 0)
            result = 0;
    } else {
        item = Inventory_GetItemId(panel->grid.selection);
        result = -1;
        if (item < 0)
            result = 0;
    }
    return result;
}
