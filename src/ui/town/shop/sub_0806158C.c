#include "ui/town/shop_states.h"
void Shop_BeginMoveSelection(struct ShopControlView *panel)
{
    Shop_BeginActionSelection(panel, 0);
    panel->interactionFlags |= 16;
    if (panel->pageCount > 1)
        panel->interactionFlags |= 1;
    SceneInput_SetGridFlags(&panel->grid, 0x1C);
    Shop_UpdateModePrompt(panel, 1);
}
