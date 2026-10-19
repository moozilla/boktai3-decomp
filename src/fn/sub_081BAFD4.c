#define MenuCursor_UpdateState sub_081BAFD4
#include "global.h"

struct MenuCursorEntry {
    u32 unknown0[3];
    u16 unknownC;
    u16 flags;
};

struct MenuCursor {
    u8 unknown0[0x20];
    u32 actorFlags;
    u8 unknown24[0x7c];
    s16 selectedIndex;
    s16 entryCount;
    struct MenuCursorEntry entries[16];
    u32 stateFlags;
    void *callback;
};

s32 Callback_InvokeActor(void *arg, void *function);

static inline void MenuCursor_ClearRequests(struct MenuCursor *cursor)
{
    cursor->stateFlags = 0;
}

/* Tentative: callback requests state bits; entry bit 1 cancels state bit 1. */
void sub_081BAFD4(struct MenuCursor *cursor)
{
    u32 *state;
    MenuCursor_ClearRequests(cursor);
    if (cursor->callback)
        Callback_InvokeActor(cursor, cursor->callback);
    {
        u32 one = 1;
        u32 requested = cursor->stateFlags & one;
        state = (u32 *)((u32)cursor + 0x1a4);
        if (requested)
            cursor->actorFlags |= one;
        else
            cursor->actorFlags &= ~1;
    }
    if (cursor->selectedIndex >= 0) {
        struct MenuCursorEntry *entry =
            (struct MenuCursorEntry *)((cursor->selectedIndex << 4) + (u32)cursor + 0xa4);
        u32 mask = 2;
        if (entry->flags & mask) {
            u32 clear = ~2;
            *state = (*state & clear) | 4;
        }
    } else {
        u32 clear = ~2;
        *state &= clear;
    }
}
