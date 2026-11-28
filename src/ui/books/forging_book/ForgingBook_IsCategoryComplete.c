#include "global.h"
#include "ui/books/forging_book.h"

u32 ForgingBook_IsCategoryComplete(struct ForgingBook *book, s32 category)
{
    return ForgingBook_CountDiscoveredCategoryEntries(book, category) >=
           ForgingBook_GetCategoryEntryCount(category);
}
