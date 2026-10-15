#include "scene_inventory_actions.h"
s32 SceneInventory_FindEquippedSlot(s32 selected)
{
    s32 i = 0;
    s16 *slot = ((struct SceneInputEquipmentView *)gUnk_02000710)->primary;
    for (i = 0; i < 8; slot++, i++) {
        if (*slot == selected)
            return i;
    }
    return -1;
}
