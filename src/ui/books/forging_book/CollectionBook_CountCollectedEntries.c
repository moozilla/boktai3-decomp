#include "global.h"
#include "ui/books/collection_book.h"

s32 CollectionBook_CountCollectedEntries(void)
{
    s32 count = 0;
    s32 i = 0;
    s32 collected = gGameState->flags596;

    for (; i < 10; i++) {
        if ((collected >> i) & 1)
            count++;
    }
    return count;
}
