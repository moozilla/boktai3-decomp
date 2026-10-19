#include "global.h"
#ifdef NONMATCHING
// fuzzy: 90.3%
// foundry draft (worker best 90.3%); what differs is mostly register allocation and instruction order
#define EffectPool_Destroy sub_0813AD08
#define gEffectPool        gUnk_020001E0
struct EffectPoolEntry {
    u8 pad00[0x17];
    s8 effectIndex;
    u8 pad18[8];
    u32 active;
};
struct PooledEffect {
    u8 bytes[0x28];
};
struct EffectPool {
    u8 pad00[0x1c];
    struct PooledEffect effects[32];
    struct EffectPoolEntry entries[32];
};
extern struct EffectPool *gUnk_020001E0;
void Effect_Remove(void *);
static inline void ClearEffectPool(void)
{
    gUnk_020001E0 = 0;
}
s32 sub_0813AD08(struct EffectPool *pool)
{
    struct EffectPoolEntry *entry = pool->entries;
    s32 count = 31;
    do {
        if (entry->active)
            Effect_Remove(&pool->effects[entry->effectIndex]);
        entry++;
        count--;
    } while (count >= 0);
    ClearEffectPool();
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0813AD08);
#endif
