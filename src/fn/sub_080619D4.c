#include "scene_inventory_states.h"
void SceneInventory_BeginCatalogTransition(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0x80;
    SceneInput_SaveSelection(&panel->grid);
    SceneInventory_SelectActionDestination(panel);
    switch (panel->category) {
        case 0:
            SceneInventory_BeginEquipmentCatalogMove((void *)panel);
            break;
        case 1:
            SceneInventory_BeginItemCatalogMove((void *)panel);
            break;
        case 2:
            SceneInventory_BeginConsumableCatalogMove((void *)panel);
            break;
    }
    SceneInventory_SetControllerCallback(
        panel, (SceneInventoryCallback)SceneInventory_TickPurchaseTransition);
}
