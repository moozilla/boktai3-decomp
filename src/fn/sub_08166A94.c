#include "global.h"
s32 PauseMenu_IsCategoryEntryEmpty(void *, u32, u32);
u32 PauseMenu_CanSwapCategoryEntries(void *ctx, u32 a, u32 b, u32 c)
{
    if (a == b)
        return 0;
    if (PauseMenu_IsCategoryEntryEmpty(ctx, a, c) == 0)
        return 1;
    if (PauseMenu_IsCategoryEntryEmpty(ctx, b, c) == 0)
        return 1;
    return 0;
}
