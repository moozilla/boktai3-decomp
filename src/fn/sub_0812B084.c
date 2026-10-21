// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct LargeEffectPoolSlot {
    u8 unknown[0x177];
    u8 index;
    u8 tail[0x14];
};
struct LargeEffectPool {
    u8 unknown[0x1C];
    u32 used;
    struct LargeEffectPoolSlot slots[8];
};
void Memory_Clear(void *, u32);
static inline u8 LargeEffectPool_IsUsed(u32 *used, u32 mask)
{
    if (*used & mask)
        return TRUE;
    return FALSE;
}
struct LargeEffectPoolSlot *LargeEffectPool_AllocateSlot(u8 *poolArg)
{
    register u8 *pool asm("r5") = poolArg;
    register s32 i asm("r6") = 0;
    u32 one = 1;
    u8 *index = pool + 0x197;
    u8 *slot = pool + 0x20;
    struct LargeEffectPoolSlot *result;
    do {
        if (!LargeEffectPool_IsUsed((u32 *)(pool + 0x1C), one << i)) {
            Memory_Clear(slot, 0x18C);
            asm("" : "+r"(i));
            *(u32 *)(pool + 0x1C) |= one << i;
            *index = i;
            result = (struct LargeEffectPoolSlot *)slot;
            goto done;
        }
        index += 0x18C;
        slot += 0x18C;
        i++;
    } while (i <= 7);
    result = NULL;
done:
    return result;
}
