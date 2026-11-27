#ifndef GUARD_INCLUDE_OBJECTS_ITEMS_SPRITE_DROP_SMALL_CALLBACK_POOL_LAYOUT_H
#define GUARD_INCLUDE_OBJECTS_ITEMS_SPRITE_DROP_SMALL_CALLBACK_POOL_LAYOUT_H

#include "global.h"
#include "objects/items/small_callback_entry_layout.h"

struct SmallCallbackPool {
    u8 pad0[0x18];
    struct SmallCallbackEntry entries[16];
};

typedef char SmallCallbackPoolSizeCheck[sizeof(struct SmallCallbackPool) == 0xb98 ? 1 : -1];

#endif
