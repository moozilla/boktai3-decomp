#include "scene_inventory_states.h"
void SceneInventory_BeginRemovalTransition(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0x80;
    switch (panel->category) {
        case 0:
            SceneInventory_BeginEquipmentRemoveMove((void *)panel);
            break;
        case 1:
            SceneInventory_BeginItemRemoveMove((void *)panel);
            break;
        case 2:
            SceneInventory_BeginConsumableRemoveMove((void *)panel);
            break;
    }
    SceneInventory_SetControllerCallback(
        panel, (SceneInventoryCallback)SceneInventory_TickDiscardTransition);
}
