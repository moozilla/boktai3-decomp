#include "ui/town/shop_states.h"
void Shop_BeginItemSelection(struct ShopControlView *panel)
{
    Shop_BeginActionSelection(panel, 1);
    SceneInput_SetGridFlags(&panel->grid, 0x1E);
    Shop_UpdateModePrompt(panel, 1);
}
