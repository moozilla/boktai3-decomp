#define EffectPool_FindFreeSlot sub_081B7340
#include "global.h"
struct EffectPoolSlot {
    s32 active;
    u8 pad4[0x48];
};
struct EffectPool {
    u8 task[0x18];
    s32 count;
    struct EffectPoolSlot slots[8];
};
struct EffectPoolSlot *sub_081B7340(struct EffectPool *pool)
{
    s32 *slot;
    s32 offset;
    s32 end;
    if (pool->count <= 7)
        goto scan;
    goto fail;
found:
    return (struct EffectPoolSlot *)((u8 *)pool + (offset + 0x1c));
scan:
    slot = (s32 *)pool->slots;
    offset = 0;
    end = (s32)pool + 0x230;
    do {
        if (*slot <= 0)
            goto found;
        slot += 19;
        offset += 0x4c;
    } while ((s32)slot <= end);
fail:
    return 0;
}
