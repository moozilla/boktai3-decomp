#include "global.h"
#include "objects/items/sprite_drop/small_callback_entry_layout.h"
struct SmallCallbackPool {
    u8 pad0[0x18];
    struct SmallCallbackEntry entries[16];
};
s32 Callback_InvokeActor(void *, void *);
s32 CallbackPool_InvokeActiveSmall(struct SmallCallbackPool *pool)
{
    s32 i;
    for (i = 0; i <= 15; i++) {
        if (pool->entries[i].active) {
            void **callback;
            u32 offset;
            struct SmallCallbackEntry *entry;
            callback = &pool->entries[0].callback;
            callback += i * (0xB8 / 4);
            offset = i * 0xB8 + 0x18;
            entry = (struct SmallCallbackEntry *)((u32)pool + offset);
            Callback_InvokeActor(entry, *callback);
        }
    }
    return 0;
}
