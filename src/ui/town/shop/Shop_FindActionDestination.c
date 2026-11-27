#include "ui/town/shop_control.h"
s32 Shop_FindActionDestination(struct ShopControlView *panel)
{
    switch (panel->category) {
        case 0:
            return Shop_FindEmptyEquipmentSlot(panel);
        case 1:
            return Shop_FindEmptyItemSlot(panel);
        case 2:
            return Shop_FindEmptyConsumableSlot(panel);
    }
    return 0;
}
