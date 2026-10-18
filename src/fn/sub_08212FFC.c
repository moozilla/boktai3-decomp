#include "global.h"
s32 CollectionBook_CountCollectedEntries(void);
u32 CollectionBook_IsComplete(void)
{
    s32 v = CollectionBook_CountCollectedEntries();
    u32 r = 0;
    if (v > 9)
        r = 1;
    return r;
}
