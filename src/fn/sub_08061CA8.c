#include "scene_inventory_control.h"
#define SceneInventory_UpdateMixedCatalogCategory sub_08061CA8
u32 SceneInventory_UpdateMixedCatalogCategory(struct SceneInventoryControlView *panel)
{
    u32 destination = 0;
    if (panel->catalogMode == 3) {
        if (panel->page < panel->itemPageCount)
            panel->category = 1;
        else
            panel->category = 2;
        destination = sub_0806143C(panel);
        panel->destination = destination;
    }
}
