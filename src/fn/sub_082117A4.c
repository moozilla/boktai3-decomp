#include "global.h"
s32 sub_0821172C(u32);
s32 ForgingBook_GetCategoryEntryCount(u32);
u32 ForgingBook_IsCategoryComplete(u32 a, u32 b)
{
    s32 x = sub_0821172C(a);
    s32 y = ForgingBook_GetCategoryEntryCount(b);
    u32 r = 0;
    if (x >= y)
        r = 1;
    return r;
}
