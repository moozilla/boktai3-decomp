#include "global.h"
#ifdef NONMATCHING
// fuzzy: 92.6%
// foundry draft (worker best 92.6%); what differs is mostly register allocation and instruction order
#define CallbackPool_InvokeActiveSmall sub_08115F74
struct SmallCallbackEntry {
    u8 payload[0xB0];
    u8 active;
    u8 padB1[3];
    void *callback;
};
struct SmallCallbackPool {
    u8 pad0[0x18];
    struct SmallCallbackEntry entries[16];
};
s32 Callback_InvokeActor(void *, void *);
s32 sub_08115F74(struct SmallCallbackPool *pool)
{
    s32 i;
    for (i = 0; i <= 15; i++) {
        if (pool->entries[i].active) {
            void **callback = &pool->entries[0].callback + i * (0xB8 / 4);
            u32 offset = i * 0xB8 + 0x18;
            struct SmallCallbackEntry *entry = (struct SmallCallbackEntry *)((u32)pool + offset);
            Callback_InvokeActor(entry, *callback);
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08115F74);
#endif
