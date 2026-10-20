#include "scene_inventory_states.h"
void SceneInventory_BeginSwapTransition(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0x80;
    switch (panel->category) {
        case 0:
            SceneInventory_BeginEquipmentSwapPair((void *)panel);
            break;
        case 1:
            SceneInventory_BeginItemSwapPair((void *)panel);
            break;
        case 2:
            SceneInventory_BeginConsumableSwapPair((void *)panel);
            break;
    }
    SceneInventory_SetControllerCallback(panel,
                                         (SceneInventoryCallback)SceneInventory_TickSwapTransition);
}
