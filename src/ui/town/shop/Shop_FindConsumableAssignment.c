#include "global.h"

struct ShopSlotsView {
    u8 unknown0[0x58];
    s16 slots[4];
};
extern struct ShopSlotsView *gGameState;

s32 Shop_FindConsumableAssignment(s32 slot)
{
    s32 i = 0;
    s16 *p = gGameState->slots;
    for (; i < 4; p++, i++) {
        if (*p == slot)
            return i;
    }
    return -1;
}
