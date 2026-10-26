// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
// 4 differing instruction runs; first diff at +0x1C (mov r0, r8 vs bl #0x820112c)
struct Pair {
    u32 a, b;
};
struct Pool {
    u8 data[0x9ec];
};
struct Pos {
    u16 x, y, z, pad;
};
u8 *PlayerClone_FindFreeSlot(void *, s32 *);
void Memory_Copy(void *, void *, u32);
void Anim_LoadKeyframe(void *, void *, u32, u32);
u8 *Player_GetEffectivePosition(u32);
void PlayerClone_SetBehaviorCallback(void *, void *);
void PlayerClone_UpdateBehavior(void);
void PlayerClone_CreatePool(void);
static inline void CpuSetW(const void *src, void *dest, u32 control)
{
    CpuSet(src, dest, control);
}
s32 sub_08201160(u8 *resource, struct Pos *pos)
{
    u8 *t;
    struct {
        u32 fill;
        s32 index;
    } scratch;
    u8 *pool;
    t = gDirectionalSpritePool;
    pool = t;
    asm("" : "+r"(pool));
    if (t) {
        u8 *obj = PlayerClone_FindFreeSlot(((struct Pool *)pool)->data, &scratch.index);
        if (obj) {
            void *handler = obj + 0x198;
            void *display;
            u32 zero;
            u8 *tmp;
            Memory_Copy(handler, resource, 0x20);
            zero = 0;
            *(u32 *)obj = zero;
            *(u32 *)(obj + 0x308) = zero;
            *(u16 *)(obj + 0x330) = zero;
            *(u32 *)(obj + 0x31c) = zero;
            display = obj + 0x1b8;
            Anim_LoadKeyframe(display, obj + 0x278, 0, 1);
            *(u32 *)(obj + 0x324) = *(u32 *)(resource + 8);
            obj[0x1d2] = 2;
            *(struct Pair *)(obj + 0x1d8) = *(struct Pair *)pos;
            *(struct Pair *)(obj + 0x1c) = *(struct Pair *)pos;
            *(u16 *)(obj + 0x14) = pos->x;
            *(u16 *)(obj + 0x16) = pos->y + 0x80;
            *(u16 *)(obj + 0x18) = pos->z;
            tmp = Player_GetEffectivePosition(0);
            *(struct Pair *)(obj + 0x24) = *(struct Pair *)tmp;
            {
                void *target = obj + 0xc;
                scratch.fill = zero;
                CpuSetW(&scratch.fill, target, 0x05000002);
            }
            *(u32 *)((u8 *)display + 8) |= 1;
            PlayerClone_SetBehaviorCallback(handler, PlayerClone_UpdateBehavior);
            *(u32 *)(pool + 0x18) |= 1 << scratch.index;
            (*(u32 *)(pool + 0x9e8))++;
            return scratch.index;
        }
    } else
        PlayerClone_CreatePool();
    return -1;
}
