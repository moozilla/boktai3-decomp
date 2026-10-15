#include "global.h"

struct SceneInventorySlotsView {
    u8 unknown0[0x58];
    s16 slots[4];
};
extern struct SceneInventorySlotsView *gUnk_02000710;

s32 SceneInventory_FindConsumableAssignment(s32 slot)
{
    s32 i = 0;
    s16 *p = gUnk_02000710->slots;
    for (; i < 4; p++, i++) {
        if (*p == slot)
            return i;
    }
    return -1;
}
