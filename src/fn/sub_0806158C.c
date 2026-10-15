#include "scene_inventory_states.h"
void SceneInventory_BeginMoveSelection(struct SceneInventoryControlView *panel)
{
    SceneInventory_BeginActionSelection(panel, 0);
    panel->interactionFlags |= 16;
    if (panel->pageCount > 1)
        panel->interactionFlags |= 1;
    SceneInput_SetGridFlags(&panel->grid, 0x1C);
    SceneInventory_UpdateModePrompt(panel, 1);
}
