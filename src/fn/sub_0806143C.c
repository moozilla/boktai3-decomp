#include "scene_inventory_control.h"
s32 SceneInventory_FindActionDestination(struct SceneInventoryControlView *panel)
{
    switch (panel->category) {
        case 0:
            return SceneInventory_FindEmptyEquipmentSlot(panel);
        case 1:
            return SceneInventory_FindEmptyItemSlot(panel);
        case 2:
            return SceneInventory_FindEmptyConsumableSlot(panel);
    }
    return 0;
}
