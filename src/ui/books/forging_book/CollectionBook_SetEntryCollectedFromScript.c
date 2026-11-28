#include "global.h"
#include "ui/books/collection_book.h"

void CollectionBook_SetEntryCollectedFromScript(void)
{
    if (Script_SeekToKeyword('i')) {
        u32 entry = Script_GetValue();

        gGameState->flags596 |= 1 << entry;
    }
}
