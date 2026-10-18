#include "global.h"
#define EffectPool_AllocateSlot sub_0818F218

struct EffectPoolSlot {
    u8 unknown[0x6C];
};
struct EffectPool {
    struct EffectPoolSlot slots[24];
    u32 occupied;
};

struct EffectPoolSlot *sub_0818F218(struct EffectPool *pool)
{
    struct EffectPoolSlot *slot = pool->slots;
    u32 bit = 1;
    volatile u32 *occupied = &pool->occupied;
    while (bit <= 0xFFFFFF) {
        u32 flags = *occupied;
        if (!(flags & bit)) {
            *occupied = flags | bit;
            return slot;
        }
        slot++;
        bit <<= 1;
    }
    return 0;
}
