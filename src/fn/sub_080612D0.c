#include "scene_inventory_control.h"
s32 SceneInventory_ClassifySelectedAction(struct SceneInventoryControlView *panel)
{
    switch (panel->category) {
        case 0:
            return SceneInventory_ClassifyEquipmentAction(panel);
        case 1:
            return SceneInventory_ClassifyItemAction(panel);
        case 2:
            return SceneInventory_ClassifyConsumableAction(panel);
    }
}
