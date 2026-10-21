// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define gPairSlotStateCallbacks gUnk_08610734
struct PairSlot {
    u8 unknown[0xDE];
    u8 index;
    u8 state;
    u8 padding[4];
};
struct PairSlotPool {
    u8 unknown[0x18];
    struct PairSlot entries[2];
    u32 allocated;
};
extern void *gUnk_08610734[];
s32 sub_08249240(void *, void *, void *);
static inline u8 PairSlotPool_IsAllocated(struct PairSlotPool *pool, s32 index)
{
    if (pool->allocated & (1 << index))
        return TRUE;
    return FALSE;
}
s32 PairSlotPool_Update(struct PairSlotPool *pool)
{
    s32 index;
    if (((u32 *)pool)[0x78]) {
        asm("" ::: "r1");
        for (index = 0; index < 2; index++) {
            if (PairSlotPool_IsAllocated(pool, index)) {
                struct PairSlot *entry = &pool->entries[index];
                sub_08249240(pool, entry, gUnk_08610734[entry->state]);
            }
        }
    }
    return 0;
}
