#include "scene_inventory_states.h"
void SceneInventory_BeginSwapTransition(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0x80;
    switch (panel->category) {
        case 0:
            sub_0806055C((void *)panel);
            break;
        case 1:
            sub_08060DB4((void *)panel);
            break;
        case 2:
            sub_08060A70((void *)panel);
            break;
    }
    SceneInventory_SetControllerCallback(panel,
                                         (SceneInventoryCallback)SceneInventory_TickSwapTransition);
}
