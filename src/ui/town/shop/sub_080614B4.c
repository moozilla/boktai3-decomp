#include "ui/town/shop_control.h"
void Shop_BeginActionSelection(struct SceneInventoryControlView *panel, s32 mode)
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
    panel->destination = Shop_FindActionDestination(panel);
    Shop_UpdateModePrompt(panel, 0);
    Shop_SetControllerCallback(
        panel, (SceneInventoryCallback)Shop_TickActionSelection);
}
