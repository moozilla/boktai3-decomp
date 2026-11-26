#include "ui/town/shop_states.h"
void Shop_BeginRemovalTransition(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0x80;
    switch (panel->category) {
        case 0:
            Shop_BeginEquipmentRemoveMove((void *)panel);
            break;
        case 1:
            Shop_BeginItemRemoveMove((void *)panel);
            break;
        case 2:
            Shop_BeginConsumableRemoveMove((void *)panel);
            break;
    }
    Shop_SetControllerCallback(
        panel, (SceneInventoryCallback)Shop_TickDiscardTransition);
}
