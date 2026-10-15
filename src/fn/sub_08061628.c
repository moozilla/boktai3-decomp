#include "scene_inventory_states.h"
#define SceneInventory_BeginReposition sub_08061628
void SceneInventory_BeginReposition(struct SceneInventoryControlView *panel)
{
    struct SceneInputGridView *grid = &panel->grid;
    SceneInput_SaveSelection(grid);
    panel->interactionFlags = 0x2C;
    panel->display.category.position = panel->display.compareA.position;
    SceneInput_SetGridFlags(grid, 0x1E);
    sub_080613BC(panel, 1);
    sub_0806130C(panel, (SceneInventoryCallback)sub_08062190);
}
