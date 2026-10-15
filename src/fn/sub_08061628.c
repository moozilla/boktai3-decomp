#include "scene_inventory_states.h"
void SceneInventory_BeginReposition(struct SceneInventoryControlView *panel)
{
    struct SceneInputGridView *grid = &panel->grid;
    SceneInput_SaveSelection(grid);
    panel->interactionFlags = 0x2C;
    panel->display.category.position = panel->display.compareA.position;
    SceneInput_SetGridFlags(grid, 0x1E);
    SceneInventory_UpdateModePrompt(panel, 1);
    SceneInventory_SetControllerCallback(panel,
                                         (SceneInventoryCallback)SceneInventory_TickMoveSelection);
}
