#include "global.h"
struct G {
    u8 pad[0x596];
    s16 f596;
};
extern struct G *gGameState;
s32 CollectionBook_IsEntryCollected(s32 n)
{
    return gGameState->f596 & (1 << n);
}
