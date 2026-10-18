#include "global.h"

/* Tentative: bit 0 suppresses display of an uncollected entry. */
struct CollectionBookEntryDisplay {
    u32 flags;
    u8 unknown04[0x28];
};

struct CollectionBook {
    u8 unknown000[0x500];
    struct CollectionBookEntryDisplay entries[10];
    u8 unknown6B8[0x28];
    s16 entryIds[10];
    s16 page;
};

s32 CollectionBook_IsEntryCollected(s32 n);

s32 CollectionBook_RefreshCollectedEntryVisibility(struct CollectionBook *book)
{
    s32 i;
    u32 hidden = 1;
    struct CollectionBookEntryDisplay *entry = book->entries;
    for (i = 9; i >= 0; i--) {
        entry->flags |= hidden;
        entry++;
    }
    for (i = book->page * 10; i < book->page * 10 + 10; i++) {
        if (CollectionBook_IsEntryCollected(book->entryIds[i]))
            book->entries[i].flags &= ~1;
        if (i > 8)
            break;
    }
    return 0;
}
