#include "ui/town/shop_actions.h"
s32 Shop_FindEquippedSlot(s32 selected)
{
    s32 i = 0;
    s16 *slot = ((struct SceneInputEquipmentView *)gGameState)->primary;
    for (i = 0; i < 8; slot++, i++) {
        if (*slot == selected)
            return i;
    }
    return -1;
}
