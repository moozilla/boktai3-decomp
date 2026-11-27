#include "ui/town/shop_states.h"
void Shop_BeginSpecialTransition(struct ShopControlView *panel)
{
    panel->interactionFlags = 0x80;
    switch (panel->category) {
        case 0:
            Shop_BeginEquipmentSpecialMove((void *)panel);
            break;
        case 1:
            Shop_BeginItemSpecialMove((void *)panel);
            break;
        case 2:
            Shop_BeginConsumableSpecialMove((void *)panel);
            break;
    }
    Shop_SetControllerCallback(panel,
                                         (ShopCallback)Shop_TickSaleTransition);
}
