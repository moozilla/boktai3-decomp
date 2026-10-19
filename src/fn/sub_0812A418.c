#include "global.h"
#ifdef NONMATCHING
// fuzzy: 84.6%
// foundry draft (worker best 83.0%); what differs is mostly register allocation and instruction order
#define EffectPool_UpdateSlots sub_0812A418
struct EffectPoolSlot { u8 unknown[0xF2]; u8 state; u8 index; u8 tail[8]; };
struct EffectPool { u8 unknown[0x38]; u32 used; struct EffectPoolSlot slots[12]; u32 suspended; };
extern void *gUnk_086107B4[];
s32 sub_08249240(void *, void *, void *);
static inline u8 EffectPool_IsUsed(u32 *used, u32 mask)
{
    if (*used & mask) return TRUE;
    return FALSE;
}
s32 sub_0812A418(struct EffectPool *pool)
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
    return (s32)pool;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0812A418);
#endif
