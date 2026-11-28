#include "global.h"
#include "ui/books/collection_book.h"

/* Tentative: bit 0 suppresses display of an uncollected entry. */
s32 CollectionBook_RefreshCollectedEntryVisibility(struct CollectionBook *book)
{
    s32 i;

    for (i = 0; i < 10; i++)
        book->entries[i].flags |= 1;
    for (i = book->page * 10; i < book->page * 10 + 10; i++) {
        if (CollectionBook_IsEntryCollected(book->entryIds[i]))
            book->entries[i].flags &= ~1;
        if (i > 8)
            break;
    }
    return 0;
}
