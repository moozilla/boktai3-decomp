#include "scene_inventory_control.h"
void SceneInventory_BeginActionSelection(struct SceneInventoryControlView *panel, s32 mode)
{
    panel->interactionFlags = 100;
    switch (mode) {
        case 0:
            SceneInput_InitGrid(&panel->grid, 1, 0, 0, 28);
            break;
        case 1:
            SceneInput_InitGrid(&panel->grid, 1, 4, 0, 28);
            break;
        case 2:
            SceneInput_RestoreSelection(&panel->grid);
            break;
    }
    panel->destination = SceneInventory_FindActionDestination(panel);
    SceneInventory_UpdateModePrompt(panel, 0);
    SceneInventory_SetControllerCallback(
        panel, (SceneInventoryCallback)SceneInventory_TickActionSelection);
}
