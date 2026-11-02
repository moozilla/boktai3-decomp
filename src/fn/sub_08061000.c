#include "scene_inventory_control.h"
s32 SceneInventory_ClassifyItemMove(struct SceneInventoryControlView *panel)
{
    s32 second;
    if (panel->grid.savedSelection <= 15) {
        s32 first = Inventory_GetItemId(panel->grid.savedSelection);
        second = Inventory_GetItemId(panel->grid.selection);
        if (panel->grid.savedSelection == panel->grid.selection)
            return 0;
        if (first < 0 && second < 0)
            return 0;
        if (panel->mode == 1)
            return 1;
    } else {
        if (panel->grid.selection == panel->grid.savedSelection) {
            SceneInventory_SelectActionDestination(panel);
            return 4;
        }
        if (panel->grid.selection <= 15) {
            second = Inventory_GetItemId(panel->grid.selection);
            if (second < 0)
                return 4;
        }
    }
    return 0;
}
