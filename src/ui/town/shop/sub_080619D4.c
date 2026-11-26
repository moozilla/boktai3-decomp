#include "ui/town/shop_states.h"
void Shop_BeginCatalogTransition(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0x80;
    SceneInput_SaveSelection(&panel->grid);
    Shop_SelectActionDestination(panel);
    switch (panel->category) {
        case 0:
            Shop_BeginEquipmentCatalogMove((void *)panel);
            break;
        case 1:
            Shop_BeginItemCatalogMove((void *)panel);
            break;
        case 2:
            Shop_BeginConsumableCatalogMove((void *)panel);
            break;
    }
    Shop_SetControllerCallback(
        panel, (SceneInventoryCallback)Shop_TickPurchaseTransition);
}
