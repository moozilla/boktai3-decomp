#include "global.h"
s32 Collection_CountUnlockedEntries(void);
u32 Collection_IsComplete(void)
{
    s32 v = Collection_CountUnlockedEntries();
    u32 r = 0;
    if (v > 26)
        r = 1;
    return r;
}
