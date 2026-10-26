#include "global.h"
struct G {
    u8 pad[0x596];
    s16 f596;
};
extern struct G *gGameState;
s32 CollectionBook_CountCollectedEntries(void)
{
    s32 n = 0;
    s32 i = 0;
    s32 v = gGameState->f596;
    do {
        if ((v >> i) & 1)
            n++;
        i++;
    } while (i <= 9);
    return n;
}
