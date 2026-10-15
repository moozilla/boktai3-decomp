#include "scene_inventory_states.h"
#define SceneInventory_BeginConsumableSelection sub_08061600
void SceneInventory_BeginConsumableSelection(struct SceneInventoryControlView *panel)
{
    sub_080614B4(panel, 1);
    SceneInput_SetGridFlags(&panel->grid, 0x1E);
    sub_080613BC(panel, 1);
}
