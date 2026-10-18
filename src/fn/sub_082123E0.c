#include "global.h"
#define SceneGrid_IsCellBeforeLimit sub_082123E0
s32 ForgingBook_GetCategoryEntryCount(u32);
s32 sub_082123E0(s32 column, s32 row, s32 selection)
{
    s32 cell, limit;
    if ((u32)column > 5 || row < 0 || row > 2)
        return 0;
    if (selection > 5) {
        cell = row << 1;
        cell += row;
        cell <<= 1;
        cell += column;
        limit = ForgingBook_GetCategoryEntryCount(selection) - 1;
    } else {
        cell = row << 1;
        cell += row;
        cell <<= 1;
        cell += column;
        limit = ForgingBook_GetCategoryEntryCount(selection);
    }
    if (cell < limit)
        return 1;
    return 0;
}
