#include "scene_inventory_states.h"
#define SceneInventory_BeginAlternateItemSelection sub_08061B94
void SceneInventory_BeginAlternateItemSelection(struct SceneInventoryControlView *panel)
{
    sub_080614B4(panel, 2);
    panel->interactionFlags |= 0x80;
    SceneInput_SetGridFlags(&panel->grid, 0x1E);
    sub_080613BC(panel, 1);
}
