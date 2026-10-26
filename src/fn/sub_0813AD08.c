#include "global.h"
#include "ram.h"
#define gEffectPool gScreenMoteManager
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
void Effect_Remove(void *);
s32 EffectPool_Destroy(struct EffectPool *pool)
{
    struct EffectPoolEntry *entry = pool->entries;
    s32 count = 31;
    do {
        if (entry->active)
            Effect_Remove(&pool->effects[entry->effectIndex]);
        entry++;
        count--;
    } while (count >= 0);
    {
        struct EffectPool *cleared = 0;
        gScreenMoteManager = cleared;
    }
    return 0;
}
