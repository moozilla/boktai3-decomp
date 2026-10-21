#include "global.h"
#include "callback_pool.h"

// foundry draft (worker best 98.4%); what differs is mostly register allocation and instruction order
#define gCallbackPoolUpdateTable gUnk_086064E8

extern void *gUnk_086064E8[];
s32 sub_08249240(void *, void *, void *);
static inline u8 CallbackPool_IsActive(u32 *mask, s32 i)
{
    if (*mask & (1 << i))
        return TRUE;
    return FALSE;
}
s32 CallbackPool_UpdateEntries(struct CallbackPool *pool)
{
    struct CallbackPoolEntry *start = pool->entries;
    struct CallbackPoolEntry *entry;
    s32 i;
    if (!pool->activeMask)
        return 0;
    for (i = 0; i < 4; i++) {
        entry = &start[i];
        if (CallbackPool_IsActive(&pool->activeMask, i))
            sub_08249240(pool, entry, gUnk_086064E8[pool->entries[i].callbackIndex]);
    }
    return 0;
}
