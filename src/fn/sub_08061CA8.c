#include "scene_inventory_control.h"
u32 SceneInventory_UpdateMixedCatalogCategory(struct SceneInventoryControlView *panel)
{
    u32 destination = 0;
    if (panel->catalogMode == 3) {
        if (panel->page < panel->itemPageCount)
            panel->category = 1;
        else
            panel->category = 2;
        destination = SceneInventory_FindActionDestination(panel);
        panel->destination = destination;
    }
}
