#define ResourceSlotPool_Initialize sub_08122D58
#define ResourceSlotPool_LoadResource sub_08122D20
#define gResourceSlotPool gUnk_02000140
#include "global.h"
struct ResourceSlotSlot { u8 unknown[0x142]; u16 flags; };
struct ResourceSlotPool { u8 unknown[0x38]; struct ResourceSlotSlot slots[4]; u32 unknown548; u32 used; };
extern struct ResourceSlotPool *gUnk_02000140;
void sub_08122D20(struct ResourceSlotPool *);
s32 sub_08122D58(struct ResourceSlotPool *pool)
{
    s32 mask;
    u16 *flags;
    s32 i;
    u32 stride;
    gResourceSlotPool = pool;
    ResourceSlotPool_LoadResource(pool);
    mask = 0xffff;
    flags = &pool->slots[0].flags;
    stride = sizeof(struct ResourceSlotSlot);
    i = 3;
    do {
        *flags = mask | *flags;
        flags = (u16 *)((u8 *)flags + stride);
    } while (--i >= 0);
    pool->used = 0;
    return 0;
}
