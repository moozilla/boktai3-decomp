#include "ui/town/shop_states.h"
void Shop_BeginAlternateConsumableSelection(struct SceneInventoryControlView *panel)
{
    Shop_BeginActionSelection(panel, 2);
    panel->interactionFlags |= 0x80;
    SceneInput_SetGridFlags(&panel->grid, 0x1E);
    Shop_UpdateModePrompt(panel, 1);
}
