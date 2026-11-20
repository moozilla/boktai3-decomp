#include "sprite_projectile.h"
#include "global.h"
#include "engine.h"
#include "ram.h"

extern u32 gUnk_02000560;
extern void *gUnk_08610770[];
s32 sub_08249240(void *, void *, void *);
static inline u8 Occupied(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return TRUE;
    return FALSE;
}
s32 StateSlotPool_Update(struct ProjectilePool *input)
{
    struct ProjectilePool *pool = input;
    struct Projectile *slots = pool->entries;
    s32 i;
    if (pool->allocated) {
        u32 mask = 0x400;
        if (!((gEngineFlags | gScenePauseFlags) & mask)) {
            u32 blockedMask = 4;
            u32 blocked = gUnk_02000560 & blockedMask;
            if (!blocked) {
                u32 *used;
                void **table;
                i = 0;
                {
                    u16 usedOffset = (u32) & ((struct ProjectilePool *)0)->allocated;
                    used = (void *)((u32)slots + usedOffset - 0x18);
                }
                table = gUnk_08610770;
                do {
                    if (Occupied(used, 1 << i)) {
                        struct Projectile *slot = (void *)((u32)i * sizeof(*slots) + (u32)slots);
                        sub_08249240(pool, slot, table[slot->active]);
                    }
                    i++;
                } while (i < 10);
            }
        }
    }
    return 0;
}
