#include "global.h"
#include "ui/books/collection_book.h"
extern const u32 gUnk_0861404C[];

u32 CollectionBook_GetPageEntryCount(u32 page)
{
    return gUnk_0861404C[page];
}
