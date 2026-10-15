#include "scene_inventory_states.h"
void SceneInventory_BeginItemSelection(struct SceneInventoryControlView *panel)
{
    SceneInventory_BeginActionSelection(panel, 1);
    SceneInput_SetGridFlags(&panel->grid, 0x1E);
    SceneInventory_UpdateModePrompt(panel, 1);
}
