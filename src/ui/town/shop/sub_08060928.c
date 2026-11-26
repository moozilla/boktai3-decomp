#include "global.h"

struct ShopSlotsView {
    u8 unknown0[0x160];
    s16 slots[16];
};
extern struct ShopSlotsView *gGameState;

s32 Shop_FindEmptyConsumableSlot(const void *unusedOwner)
{
    s32 i = 0;
    s16 *p = gGameState->slots;
    for (i = 0; i < 16; p++, i++) {
        if (*p < 0)
            return i;
    }
    return -1;
}
