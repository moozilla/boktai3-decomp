#include "global.h"
#define SmallEffectPool_Allocate SceneStatusIndicator_Allocate
struct SmallEffectSlot {
    u8 unknown[0x2B];
    u8 index;
    u8 padding[4];
};
struct SmallEffectPool {
    u8 unknown[0x1C];
    struct SmallEffectSlot entries[12];
    u32 allocated;
};
static inline u8 SmallEffectPool_IsAllocated(volatile u32 *allocated, s32 one, s32 index)
{
    if (*allocated & (one << index))
        return TRUE;
    return FALSE;
}
struct SmallEffectSlot *SceneStatusIndicator_Allocate(struct SmallEffectPool *pool)
{
    s32 index = 0;
    volatile u32 *allocated = &pool->allocated;
    u16 one = 1;
    u8 *kind = &pool->entries[0].index;
    struct SmallEffectSlot *entry = pool->entries;
    do {
        if (SmallEffectPool_IsAllocated(allocated, one, index)) {
            kind += 0x30;
            entry++;
            index++;
        } else {
            u32 bit = one;
            asm("" : "+r"(bit));
            bit <<= index;
            *allocated |= bit;
            *kind = index;
            return entry;
        }
    } while (index <= 11);
    return NULL;
}
