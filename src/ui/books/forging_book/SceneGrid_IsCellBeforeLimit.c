#include "global.h"
#include "ui/books/forging_book.h"

s32 SceneGrid_IsCellBeforeLimit(s32 column, s32 row, s32 category)
{
    s32 cell, limit;

    if ((u32)column > 5 || row < 0 || row > 2)
        return 0;
    if (category > 5) {
        cell = row * 2; /* row * 6 + column, kept in one register step by step */
        cell += row;
        cell *= 2;
        cell += column;
        limit = ForgingBook_GetCategoryEntryCount(category) - 1;
    } else {
        cell = row * 2;
        cell += row;
        cell *= 2;
        cell += column;
        limit = ForgingBook_GetCategoryEntryCount(category);
    }
    if (cell < limit)
        return 1;
    return 0;
}
