#include "global.h"

// confidence=low: allocates one of four 0x144-byte slots, recording its index at 0x13E.
struct Slot144 {
    u8 unknown0[0x13e];
    u16 index;
    u8 unknown140[4];
};
struct Slot144Pool {
    u8 unknown0[0x3c];
    struct Slot144 slots[4];
    u32 occupied;
};
static inline u8 Occupied(u32 *mask, s32 index)
{
    if (*mask & (1 << index))
        return 1;
    return 0;
}
struct Slot144 *sub_0812298C(struct Slot144Pool *pool)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        if (!Occupied(&pool->occupied, i)) {
            struct Slot144 *slot = &pool->slots[i];
            pool->occupied |= 1 << i;
            slot->index = i;
            return slot;
        }
    }
    return 0;
}
