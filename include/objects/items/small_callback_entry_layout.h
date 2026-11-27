#ifndef GUARD_INCLUDE_OBJECTS_ITEMS_SPRITE_DROP_SMALL_CALLBACK_ENTRY_LAYOUT_H
#define GUARD_INCLUDE_OBJECTS_ITEMS_SPRITE_DROP_SMALL_CALLBACK_ENTRY_LAYOUT_H

#include "global.h"

struct SmallCallbackEntry {
    u8 pad0[0x1c];
    u8 sprite[4]; /* confidence=low */
    u8 spriteActive; /* confidence=low */
    u8 pad21[0x8f];
    u8 active;
    u8 padB1[0x3];
    void *callback;
};

typedef char SmallCallbackEntrySizeCheck[sizeof(struct SmallCallbackEntry) == 0xb8 ? 1 : -1];

#endif
