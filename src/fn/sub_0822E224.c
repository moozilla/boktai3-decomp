#include "global.h"
extern u8 *gGameState;
u32 GameState_IsConsumableSlotEquipped(s32 a)
{
    s32 i = 0;
    s16 *p = (s16 *)(gGameState + 0x58);
    for (; i <= 3; p++, i++) {
        if (*p == a)
            return 1;
    }
    return 0;
}
