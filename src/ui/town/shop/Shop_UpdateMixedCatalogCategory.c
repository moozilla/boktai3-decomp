#include "ui/town/shop_control.h"
u32 Shop_UpdateMixedCatalogCategory(struct ShopControlView *panel)
{
    u32 destination = 0;
    if (panel->catalogMode == 3) {
        if (panel->page < panel->itemPageCount)
            panel->category = 1;
        else
            panel->category = 2;
        destination = Shop_FindActionDestination(panel);
        panel->destination = destination;
    }
}
