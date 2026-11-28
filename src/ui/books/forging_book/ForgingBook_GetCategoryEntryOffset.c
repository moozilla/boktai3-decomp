#include "global.h"
#include "ui/books/forging_book.h"

s32 ForgingBook_GetCategoryEntryOffset(s32 category)
{
    s32 offset = 0;
    s32 i;

    if (category == 0)
        return 0;
    for (i = 0; i < category; i++)
        offset += ForgingBook_GetCategoryEntryCount(i);
    return offset;
}
