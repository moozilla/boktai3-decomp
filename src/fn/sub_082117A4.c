#include "global.h"
s32 ForgingBook_CountDiscoveredCategoryEntries(u32);
s32 ForgingBook_GetCategoryEntryCount(u32);
u32 ForgingBook_IsCategoryComplete(u32 a, u32 b)
{
    s32 x = ForgingBook_CountDiscoveredCategoryEntries(a);
    s32 y = ForgingBook_GetCategoryEntryCount(b);
    u32 r = 0;
    if (x >= y)
        r = 1;
    return r;
}
