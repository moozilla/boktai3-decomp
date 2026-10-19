
#ifdef NONMATCHING
// fuzzy: 98.4%
// foundry draft (worker best 98.4%); what differs is mostly register allocation and instruction order
#define CallbackPool_UpdateEntries sub_08104D6C
#define gCallbackPoolUpdateTable gUnk_086064E8
#include "global.h"

#include "callback_pool.h"
extern void *gUnk_086064E8[];
s32 sub_08249240(void *, void *, void *);
static inline u8 CallbackPool_IsActive(u32 *mask, s32 i)
{
    if (*mask & (1 << i)) return TRUE;
    return FALSE;
}
s32 sub_08104D6C(struct CallbackPool *pool)
{
    struct CallbackPoolEntry *start = pool->entries;
    struct CallbackPoolEntry *entry;
    s32 i;
    if (pool->activeMask) {
        for (i = 0; i < 4; i++) {
            entry = &start[i];
            if (CallbackPool_IsActive(&pool->activeMask, i))
                sub_08249240(pool, entry, gUnk_086064E8[pool->entries[i].callbackIndex]);
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08104D6C);
#endif
