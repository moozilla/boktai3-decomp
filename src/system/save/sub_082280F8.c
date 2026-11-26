#include "global.h"
extern u8 *gGameState;
s32 sub_082280F8(void)
{
    s32 best = 0;
    s32 max = -100;
    s32 i = 0;
    s16 *v = (s16 *)(gGameState + 0x666);
    for (; i <= 4; i++) {
        if (*v > max) {
            max = *v;
            best = i;
        }
        v++;
    }
    return best;
}