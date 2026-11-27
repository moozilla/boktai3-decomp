#include "ui/town/shop_states.h"
void Shop_BeginReposition(struct ShopControlView *panel)
{
    struct SceneInputGridView *grid = &panel->grid;
    SceneInput_SaveSelection(grid);
    panel->interactionFlags = 0x2C;
    panel->display.category.position = panel->display.compareA.position;
    SceneInput_SetGridFlags(grid, 0x1E);
    Shop_UpdateModePrompt(panel, 1);
    Shop_SetControllerCallback(panel,
                                         (ShopCallback)Shop_TickMoveSelection);
}
