#include "global.h"
extern u8 *gGameState;
s32 Collection_CountUnlockedEntries(void)
{
    s32 n = 0;
    s32 i = 0;
    s32 v = *(s32 *)(gGameState + 0x728);
    do {
        if ((v >> i) & 1)
            n++;
        i++;
    } while (i <= 0x1a);
    return n;
}
