#include "global.h"
#include "ui/books/collection_book.h"

s32 CollectionBook_IsEntryCollected(s32 entry)
{
    return gGameState->flags596 & (1 << entry);
}
