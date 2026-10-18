#include "global.h"
void sub_08249240(void *, void *, void *);
extern void *gUnk_086107D8[];
struct Slot {
    u8 pad[0x1B6];
    u8 state;
    u8 pad2[5];
};
struct Pool {
    u8 pad[0x38];
    u32 used;
    struct Slot slots[8];
};
static inline u8 Used(struct Pool *pool, u32 bit)
{
    if (pool->used & bit)
        return 1;
    return 0;
}
s32 sub_0812BFBC(struct Pool *input)
{
    struct Pool *pool = input;
    struct Slot *base = pool->slots;
    s32 i;
    if (pool->used) {
        void **table;
        u8 *state;
        struct Slot *slot;
        i = 0;
        table = gUnk_086107D8;
        state = &pool->slots[0].state;
        slot = base;
        do {
            if (Used(pool, 1U << i))
                sub_08249240(pool, slot, table[*state]);
            {
                u32 stride = sizeof(*slot);
                state = (void *)((u32)state + stride);
                slot = (void *)((u32)slot + stride);
            }
            i++;
        } while (i < 8);
    }
    return 0;
}
