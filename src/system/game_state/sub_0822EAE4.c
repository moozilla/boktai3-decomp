#include "global.h"
extern u8 *gGameState;
u32 SolarFruit_IsInventorySlotEquipped(s32 a)
{
    s32 i = 0;
    s16 *p = (s16 *)(gGameState + 0x90);
    for (; i <= 7; p++, i++) {
        if (*p == a)
            return 1;
    }
    return 0;
}
