#include "ui/town/shop_states.h"
void Shop_BeginSwapTransition(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0x80;
    switch (panel->category) {
        case 0:
            Shop_BeginEquipmentSwapPair((void *)panel);
            break;
        case 1:
            Shop_BeginItemSwapPair((void *)panel);
            break;
        case 2:
            Shop_BeginConsumableSwapPair((void *)panel);
            break;
    }
    Shop_SetControllerCallback(panel,
                                         (SceneInventoryCallback)Shop_TickSwapTransition);
}
