#include "scene_inventory_states.h"
#define SceneInventory_BeginItemSelection sub_080615D8
void SceneInventory_BeginItemSelection(struct SceneInventoryControlView *panel)
{
    sub_080614B4(panel, 1);
    SceneInput_SetGridFlags(&panel->grid, 0x1E);
    sub_080613BC(panel, 1);
}
