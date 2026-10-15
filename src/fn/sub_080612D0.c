#include "scene_inventory_control.h"
#define SceneInventory_ClassifySelectedAction sub_080612D0
s32 SceneInventory_ClassifySelectedAction(struct SceneInventoryControlView *panel)
{
    switch (panel->category) {
        case 0:
            return sub_08061120(panel);
        case 1:
            return sub_080611AC(panel);
        case 2:
            return sub_0806122C(panel);
    }
}
