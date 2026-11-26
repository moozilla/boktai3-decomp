#include "engine/shared/global.h"

struct LargeEffectPoolSlot {
    u8 unknown[0x177];
    u8 index;
    u8 tail[0x14];
};
struct LargeEffectPool {
    u8 unknown[0x1C];
    u32 used;
    struct LargeEffectPoolSlot slots[8];
};
void Memory_Clear(void *, u32);
static inline u8 LargeEffectPool_IsUsed(u32 *used, u32 mask)
{
    if (*used & mask)
        return TRUE;
    return FALSE;
}
struct LargeEffectPoolSlot *LargeEffectPool_AllocateSlot(u8 *poolArg)
{
    struct LargeEffectPool *pool = (void *)poolArg;
    s32 i;
    for (i = 0; i < 8; i++) {
        if (!LargeEffectPool_IsUsed(&pool->used, 1U << i)) {
            struct LargeEffectPoolSlot *slot = &pool->slots[i];
            Memory_Clear(slot, sizeof(*slot));
            pool->used |= 1U << i;
            slot->index = i;
            return slot;
        }
    }
    return NULL;
}
