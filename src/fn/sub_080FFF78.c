#include "global.h"
#ifdef NONMATCHING
// fuzzy: 92.5%
// Open: the original reloads the 0xA3C slot-mask offset into a fresh register for the loop pointer.
extern void *gUnk_0860649C[];
struct PoolSlot {
    u8 unknown0[0xC4];
    u16 state;
    u8 unknownC6[0xD8 - 0xC6];
};
struct SlotPool {
    u8 unknown0[0x1C];
    struct PoolSlot slots[12];
    u32 used;
};
static inline u8 Occupied(u32 *mask, u32 bit)
{
    if (*mask & bit)
        return TRUE;
    return FALSE;
}
s32 SlotPool_UpdateActiveSlots(struct SlotPool *pool)
{
    struct PoolSlot *slots = pool->slots;
    s32 i;
    register u32 offset asm("r1") = 0xA3C;
    u32 *guard = (u32 *)((u32)pool + offset);
    if (*guard) {
        u32 *used;
        void **table;
        i = 0;
        used = &pool->used;
        table = gUnk_0860649C;
        do {
            if (Occupied(used, 1u << i)) {
                struct PoolSlot *slot = (void *)((u32)i * sizeof(*slots) + (u32)slots);
                sub_08249240(pool, slot, table[slot->state]);
            }
            i++;
        } while (i < 12);
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", SlotPool_UpdateActiveSlots);
#endif
