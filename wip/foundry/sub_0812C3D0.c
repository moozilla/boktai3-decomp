#include "global.h"
#define CompactEffectPool_AllocateSlot sub_0812C3D0
struct CompactEffectSlot {
    u8 unknown[0xBB];
    u8 index;
    u8 tail[4];
};
struct CompactEffectPool {
    u8 unknown[0x38];
    u32 used;
    struct CompactEffectSlot slots[8];
};
static inline u8 IsUsed(u32 mask, u32 used)
{
    if (mask & used)
        return TRUE;
    return FALSE;
}
struct CompactEffectSlot *sub_0812C3D0(struct CompactEffectPool *pool)
{
    s32 i = 0;
    u32 one = 1;
    u8 *index = &pool->slots[0].index;
    struct CompactEffectSlot *slot = pool->slots;
    struct CompactEffectSlot *result;
    do {
        if (!IsUsed(one << i, pool->used)) {
            pool->used |= one << i;
            *index = i;
            result = slot;
            goto done;
        }
        index += 0xC0;
        slot++;
        i++;
    } while (i <= 7);
    result = NULL;
done:
    return result;
}
