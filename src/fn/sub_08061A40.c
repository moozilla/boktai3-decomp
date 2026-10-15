#include "scene_inventory_states.h"
void SceneInventory_BeginSpecialTransition(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0x80;
    switch (panel->category) {
        case 0:
            SceneInventory_BeginEquipmentSpecialMove((void *)panel);
            break;
        case 1:
            SceneInventory_BeginItemSpecialMove((void *)panel);
            break;
        case 2:
            SceneInventory_BeginConsumableSpecialMove((void *)panel);
            break;
    }
    SceneInventory_SetControllerCallback(panel,
                                         (SceneInventoryCallback)SceneInventory_TickSaleTransition);
}
