#include "scene_inventory_states.h"
#define SceneInventory_BeginAlternateSelection sub_08061B48
void SceneInventory_BeginAlternateSelection(struct SceneInventoryControlView *panel)
{
    sub_080614B4(panel, 2);
    panel->interactionFlags |= 144;
    if (panel->pageCount > 1)
        panel->interactionFlags |= 1;
    SceneInput_SetGridFlags(&panel->grid, 0x1C);
    sub_080613BC(panel, 1);
}
