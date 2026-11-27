#include "ui/town/shop_states.h"
void Shop_BeginBrowse(struct ShopControlView *panel)
{
    panel->interactionFlags = 0x40;
    if (panel->pageCount > 1)
        panel->interactionFlags = 0x41;
    panel->destination = Shop_FindActionDestination(panel);
    Shop_UpdateModePrompt(panel, 0);
    Shop_SetControllerCallback(panel,
                                         (ShopCallback)Shop_TickModeMenu);
}
