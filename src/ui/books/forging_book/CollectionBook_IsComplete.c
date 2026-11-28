#include "global.h"
#include "ui/books/collection_book.h"

u32 CollectionBook_IsComplete(void)
{
    return CollectionBook_CountCollectedEntries() > 9;
}
