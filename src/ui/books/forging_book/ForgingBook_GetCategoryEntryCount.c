#include "global.h"
#include "ui/books/forging_book.h"

extern u32 gUnk_08613FC0[];

s32 ForgingBook_GetCategoryEntryCount(s32 category)
{
    return gUnk_08613FC0[category];
}
