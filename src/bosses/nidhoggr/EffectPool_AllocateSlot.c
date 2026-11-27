#include "global.h"

struct EffectPoolSlot {
    u8 unknown[0x6C];
};
struct EffectPool {
    struct EffectPoolSlot slots[24];
    u32 occupied;
};

struct EffectPoolSlot *EffectPool_AllocateSlot(struct EffectPool *pool)
{
    struct EffectPoolSlot *slot = pool->slots;
    u32 bit = 1;
    while (bit <= 0xFFFFFF) {
        u32 flags = pool->occupied;
        if (!(flags & bit)) {
            pool->occupied = flags | bit;
            return slot;
        }
        slot++;
        bit <<= 1;
    }
    return 0;
}
