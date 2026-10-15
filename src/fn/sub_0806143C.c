#include "scene_inventory_control.h"
#define SceneInventory_FindActionDestination sub_0806143C
s32 SceneInventory_FindActionDestination(struct SceneInventoryControlView *panel)
{
    switch (panel->category) {
        case 0:
            return sub_08060904(panel);
        case 1:
            return sub_08060958(panel);
        case 2:
            return sub_08060928(panel);
    }
    return 0;
}
