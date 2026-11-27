#include "global.h"

struct EffectPoolSlot {
    u8 unknown[0xF3];
    u8 index;
    u8 tail[8];
};
struct EffectPool {
    u8 unknown[0x38];
    u32 used;
    struct EffectPoolSlot slots[12];
};
void Memory_Clear(void *, u32);
static inline u8 EffectPool_IsUsed(u32 *used, u32 mask)
{
    if (*used & mask)
        return TRUE;
    return FALSE;
}
struct EffectPoolSlot *EffectPoolA_AllocateSlot(u8 *poolArg)
{
    struct EffectPool *pool = (void *)poolArg;
    s32 i;
    for (i = 0; i < 12; i++) {
        if (!EffectPool_IsUsed(&pool->used, 1U << i)) {
            struct EffectPoolSlot *slot = &pool->slots[i];
            Memory_Clear(slot, sizeof(*slot));
            pool->used |= 1U << i;
            slot->index = i;
            return slot;
        }
    }
    return NULL;
}
