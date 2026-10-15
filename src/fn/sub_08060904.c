#include "scene_inventory_actions.h"

s32 SceneInventory_FindEmptyEquipmentSlot(const void *unusedOwner)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        if (sub_0822D6E0(i) == 0xff)
            return i;
    }
    return -1;
}
