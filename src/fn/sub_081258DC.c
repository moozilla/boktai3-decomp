#include "global.h"
#define PairSlotPool_Allocate sub_081258DC
struct PairSlot {
    u8 unknown[0xDE];
    u8 index;
    u8 padding[5];
};
struct PairSlotPool {
    u8 unknown[0x18];
    struct PairSlot entries[2];
    u32 allocated;
};
static inline u8 PairSlotPool_IsAllocated(struct PairSlotPool *pool, s32 index)
{
    if (pool->allocated & (1 << index)) return TRUE;
    return FALSE;
}
struct PairSlot *sub_081258DC(struct PairSlotPool *pool)
{
    s32 index;
    for (index = 0; index < 2; index++) {
        if (!PairSlotPool_IsAllocated(pool, index)) {
            struct PairSlot *entry = &pool->entries[index];
            entry->index = index;
            pool->allocated |= 1 << index;
            return entry;
        }
    }
    return NULL;
}
