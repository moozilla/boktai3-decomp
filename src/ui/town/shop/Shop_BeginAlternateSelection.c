#include "ui/town/shop_states.h"
void Shop_BeginAlternateSelection(struct ShopControlView *panel)
{
    Shop_BeginActionSelection(panel, 2);
    panel->interactionFlags |= 144;
    if (panel->pageCount > 1)
        panel->interactionFlags |= 1;
    SceneInput_SetGridFlags(&panel->grid, 0x1C);
    Shop_UpdateModePrompt(panel, 1);
}
