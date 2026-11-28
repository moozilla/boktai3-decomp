#include "ui/town/shop_control.h"
u32 Shop_UpdateMixedCatalogCategory(struct ShopControlView *panel)
{
    if (panel->catalogMode == 3) {
        if (panel->page < panel->itemPageCount)
            panel->category = 1;
        else
            panel->category = 2;
        panel->destination = Shop_FindActionDestination(panel);
    }
}
