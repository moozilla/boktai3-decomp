#include "scene_inventory_states.h"
#define SceneInventory_BeginCatalogTransition sub_080619D4
void SceneInventory_BeginCatalogTransition(struct SceneInventoryControlView *panel)
{
    panel->interactionFlags = 0x80;
    SceneInput_SaveSelection(&panel->grid);
    sub_0806147C(panel);
    switch (panel->category) {
        case 0:
            sub_080607B0((void *)panel);
            break;
        case 1:
            sub_08060F4C((void *)panel);
            break;
        case 2:
            sub_08060C54((void *)panel);
            break;
    }
    sub_0806130C(panel, (void (*)(struct SceneInventoryControlView *))sub_08062414);
}
