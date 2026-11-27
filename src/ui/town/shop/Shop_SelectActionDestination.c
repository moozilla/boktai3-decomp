#include "ui/town/shop_control.h"
void Shop_SelectActionDestination(struct ShopControlView *panel)
{
    short destination = (u16)panel->destination;
    panel->grid.selection = destination;
    {
        u32 column = 3 & destination;
        panel->grid.cursorX = column + 4;
    }
    panel->grid.cursorY = panel->grid.selection >> 2;
}
