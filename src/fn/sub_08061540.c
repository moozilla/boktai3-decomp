#include "scene_inventory_states.h"
void SceneInventory_BeginBrowse(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0x40;
    if (panel->pageCount > 1)
        panel->interactionFlags = 0x41;
    panel->destination = SceneInventory_FindActionDestination(panel);
    SceneInventory_UpdateModePrompt(panel, 0);
    SceneInventory_SetControllerCallback(panel,
                                         (SceneInventoryCallback)SceneInventory_TickModeMenu);
}
