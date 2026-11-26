#include "global.h"
struct EffectPoolSlot {
    u8 unknown[0xF2];
    u8 state;
    u8 index;
    u8 tail[8];
};
struct EffectPool {
    u8 unknown[0x38];
    u32 used;
    struct EffectPoolSlot slots[12];
    u32 suspended;
};
extern void *gUnk_086107B4[];
s32 sub_08249240(void *, void *, void *);
static inline u8 EffectPool_IsUsed(u32 *used, u32 mask)
{
    if (*used & mask)
        return TRUE;
    return FALSE;
}
s32 EffectPool_UpdateSlots(struct EffectPool *pool)
{
    s32 i;
    u8 *state;
    struct EffectPoolSlot *slot;
    if (!pool->suspended) {
        void **callbacks;
        i = 0;
        callbacks = gUnk_086107B4;
        state = &pool->slots[0].state;
        slot = pool->slots;
        do {
            if (EffectPool_IsUsed(&pool->used, 1 << i))
                sub_08249240(pool, slot, callbacks[*state]);
            state += 0xFC;
            slot++;
            i++;
        } while (i <= 11);
        return 0;
    }
    // No return value on this path: the ROM leaves the argument in r0.
}
