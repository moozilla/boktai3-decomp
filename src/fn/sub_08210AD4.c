#include "global.h"

u32 Collection_IsEntryUnlocked(u32 i);

struct CollectionEntryDisplay {
    u32 flags;
    u8 unk04[0x28];
};

struct CollectionMenu {
    u8 unk000[0x65C];
    struct CollectionEntryDisplay entries[27];
    u8 unkB00[0x80];
    s16 page;
};

/* Tentative: bit 0 hides an entry display outside the unlocked current page. */
s32 Collection_UpdateEntryVisibility(struct CollectionMenu *menu)
{
    u32 hidden = 1;
    struct CollectionEntryDisplay *entry = menu->entries;
    s32 i;

    for (i = 26; i >= 0; i--) {
        entry->flags |= hidden;
        entry++;
    }
    for (i = menu->page * 10; i < menu->page * 10 + 10; i++) {
        if (Collection_IsEntryUnlocked(i))
            menu->entries[i].flags &= ~1;
        if (i > 25)
            break;
    }
    return 0;
}
