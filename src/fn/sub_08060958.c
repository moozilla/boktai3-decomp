#include "scene_inventory_actions.h"
#define SceneInventory_FindEmptyItemSlot sub_08060958

s32 SceneInventory_FindEmptyItemSlot(const void *unusedOwner)
{
    s32 i;
    for (i = 0; i < 16; i++) {
        if (sub_0822CDA8(i) < 0)
            return i;
    }
    return -1;
}
