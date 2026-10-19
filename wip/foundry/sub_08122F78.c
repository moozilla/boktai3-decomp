#include "global.h"
#define SlotPool_Allocate sub_08122F78

struct SlotPoolEntry {
    u8 unknown[0x43];
    u8 index;
    u8 padding[4];
};
struct SlotPool {
    u8 unknown[0x20];
    struct SlotPoolEntry entries[32];
    u32 allocated;
};
static inline u8 SlotPool_IsAllocated(struct SlotPool *pool, s32 index)
{
    if (pool->allocated & (1 << index)) return TRUE;
    return FALSE;
}
struct SlotPoolEntry *sub_08122F78(struct SlotPool *pool)
{
    s32 index;
    for (index = 0; index < 32; index++) {
        if (!SlotPool_IsAllocated(pool, index)) {
            pool->entries[index].index = index;
            pool->allocated |= 1 << index;
            return (struct SlotPoolEntry *)((u8 *)&pool->entries[index].index - 0x43);
        }
    }
    return NULL;
}
