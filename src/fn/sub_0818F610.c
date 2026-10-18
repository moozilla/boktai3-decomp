#include "global.h"
struct SceneEffect {
    u8 unknown0[0x188];
};
struct SceneEffectPool {
    u8 unknown0[0xA24];
    struct SceneEffect slots[48];
    u32 occupied[2];
};
struct SceneEffect *SceneEffectPool_AllocateSlot(struct SceneEffectPool *pool)
{
    struct SceneEffect *slot = pool->slots;
    s32 index = 0;
    u32 mask = 31;
    u32 *occupied = pool->occupied;
    for (; index < 48; index++) {
        s32 wordIndex = index >> 5;
        u32 shift = index & mask;
        u32 *word = &occupied[wordIndex];
        u32 bit = 1 << shift;
        u32 flags = *word;
        if (!(flags & bit)) {
            *word = flags | bit;
            return slot;
        }
        slot++;
    }
    return 0;
}
