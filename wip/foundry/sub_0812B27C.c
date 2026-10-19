#include "global.h"
#define LargeEffectPool_Destroy sub_0812B27C
#define gLargeEffectPool        gUnk_02000180
struct LargeEffectSlot {
    u8 sprite[0xAC];
    u8 collider[0x54];
    u8 unknown[0x8C];
};
struct LargeEffectPool {
    u8 unknown[0x1C];
    u32 used;
    struct LargeEffectSlot slots[8];
};
extern struct LargeEffectPool *gUnk_02000180;
void Collider_Remove(void *);
void Sprite_Remove(void *);
static inline u8 IsUsed(u32 *used, u32 mask)
{
    if (*used & mask)
        return TRUE;
    return FALSE;
}
static inline void ClearPool(struct LargeEffectPool *value)
{
    gLargeEffectPool = value;
}
s32 sub_0812B27C(struct LargeEffectPool *pool)
{
    s32 i = 0;
    u8 *collider = pool->slots[0].collider;
    struct LargeEffectSlot *slot = pool->slots;
    do {
        if (IsUsed(&pool->used, 1 << i)) {
            Sprite_Remove(slot);
            Collider_Remove(collider);
        }
        {
            u16 stride = 0x18C;
            collider = (u8 *)((u32)collider + stride);
            slot = (struct LargeEffectSlot *)((u32)slot + stride);
        }
        i++;
    } while (i <= 7);
    ClearPool(NULL);
    return 0;
}
