#include "global.h"
s32 ForgingBook_GetCategoryEntryCount(s32);
s32 ForgingBook_GetCategoryEntryOffset(s32 n)
{
    s32 s = 0;
    s32 i;
    if (n == 0)
        return 0;
    for (i = 0; i < n; i++)
        s += ForgingBook_GetCategoryEntryCount(i);
    return s;
}
