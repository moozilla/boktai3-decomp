#include "scene_inventory_states.h"
void SceneInventory_BeginThirdSelection(struct SceneInventoryControlView *panel)
{
    SceneInventory_BeginActionSelection(panel, 3);
    panel->interactionFlags |= 0x80;
    SceneInput_SetGridFlags(&panel->grid, 0x1E);
    SceneInventory_UpdateModePrompt(panel, 1);
}
