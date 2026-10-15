#include "scene_inventory_states.h"
#define SceneInventory_BeginSpecialTransition sub_08061A40
void SceneInventory_BeginSpecialTransition(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0x80;
    switch (panel->category) {
        case 0:
            sub_08060660((void *)panel);
            break;
        case 1:
            sub_08060EA4((void *)panel);
            break;
        case 2:
            sub_08060B94((void *)panel);
            break;
    }
    sub_0806130C(panel, (void (*)(struct SceneInventoryControlView *))sub_08062634);
}
