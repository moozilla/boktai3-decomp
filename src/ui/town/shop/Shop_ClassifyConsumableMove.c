#include "ui/town/shop_control.h"
s32 Shop_ClassifyConsumableMove(struct ShopControlView *panel)
{
    s32 second;
    if (panel->grid.savedSelection <= 15) {
        s32 first = *(panel->grid.savedSelection + gGameState->table160);
        second = *(panel->grid.selection + gGameState->table160);
        if (panel->grid.savedSelection == panel->grid.selection)
            return 0;
        if (first < 0 && second < 0)
            return 0;
        if (panel->mode == 1)
            return 1;
    } else {
        if (panel->grid.selection == panel->grid.savedSelection) {
            Shop_SelectActionDestination(panel);
            return 4;
        }
        if (panel->grid.selection <= 15) {
            second = *(panel->grid.selection + gGameState->table160);
            if (second < 0)
                return 4;
        }
    }
    return 0;
}
