#include "global.h"

struct EffectPoolSlot {
    u8 unknown[0xF3];
    u8 index;
    u8 tail[8];
};
struct EffectPool {
    u8 unknown[0x38];
    u32 used;
    struct EffectPoolSlot slots[12];
};
void Memory_Clear(void *, u32);
static inline u8 EffectPool_IsUsed(u32 *used, u32 mask)
{
    if (*used & mask)
        return TRUE;
    return FALSE;
}
struct EffectPoolSlot *EffectPoolA_AllocateSlot(u8 *poolArg)
{
    u8 *pool = poolArg;
    s32 i = 0;
    u32 one = 1;
    u8 *index = pool + 0x12F;
    u8 *slot = pool + 0x3C;
    struct EffectPoolSlot *result;
    do {
        if (!EffectPool_IsUsed((u32 *)(pool + 0x38), one << i)) {
            Memory_Clear(slot, 0xFC);
            *(u32 *)(pool + 0x38) |= one << i;
            *index = i;
            result = (struct EffectPoolSlot *)slot;
            goto done;
        }
        index += 0xFC;
        slot += 0xFC;
        (*(&i))++;
    } while (i <= 11);
    result = NULL;
done:
    return result;
}
