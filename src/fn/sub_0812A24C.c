#include "global.h"
#ifdef NONMATCHING
// fuzzy: 95.8%
// foundry draft (worker best 93.9%); what differs is mostly register allocation and instruction order
#define EffectPool_AllocateSlot sub_0812A24C

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
struct EffectPoolSlot *sub_0812A24C(u8 *pool)
{
    s32 i = 0;
    u32 one = 1;
    u8 *index = pool + 0x12F;
    u8 *slot = pool + 0x3C;
    struct EffectPoolSlot *result;
    do {
        if (!EffectPool_IsUsed((u32 *)(pool + 0x38), one << i)) {
            Memory_Clear(slot, 0xFC);
            *(u32 *)(pool + 0x38) |= one << (u8)i;
            *index = i;
            result = (struct EffectPoolSlot *)slot;
            goto done;
        }
        index += 0xFC;
        slot += 0xFC;
        i++;
    } while (i <= 11);
    result = NULL;
done:
    return result;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0812A24C);
#endif
