#include "scene_inventory_states.h"
void SceneInventory_BeginAlternateConsumableSelection(struct SceneInventoryControlView *panel)
{
    SceneInventory_BeginActionSelection(panel, 2);
    panel->interactionFlags |= 0x80;
    SceneInput_SetGridFlags(&panel->grid, 0x1E);
    SceneInventory_UpdateModePrompt(panel, 1);
}
