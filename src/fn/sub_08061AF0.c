#include "scene_inventory_states.h"
#define SceneInventory_BeginRemovalTransition sub_08061AF0
void SceneInventory_BeginRemovalTransition(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0x80;
    switch (panel->category) {
        case 0:
            sub_08060708((void *)panel);
            break;
        case 1:
            sub_08060D0C((void *)panel);
            break;
        case 2:
            sub_0806097C((void *)panel);
            break;
    }
    sub_0806130C(panel, (void (*)(struct SceneInventoryControlView *))sub_08062818);
}
