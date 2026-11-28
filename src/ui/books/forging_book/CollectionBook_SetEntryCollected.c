// COMPILER: old_agbcc
/* CollectionBook_SetEntryCollected: Sets the indexed bit in the persistent ten-entry collection mask */

#include "global.h"
#include "ui/books/collection_book.h"

void CollectionBook_SetEntryCollected(s32 entry)
{
    gGameState->flags596 |= 1 << entry;
}
