#include "global.h"
struct Pair339 {
    u32 a, b;
};
u32 AnimData_FindEntry(u32);
void sub_08217DE0(u8 *, u32, s32);
void Effect_SetOrigin(u8 *, s32, s32);
void SpriteDrawable_SelectTileFrame(u8 *, u32, s32);
void ChargedBurst_InitCoreEffect(u8 *p)
{
    u8 *e = p + 0x74;
    u32 resource = AnimData_FindEntry(0x1c1a);
    *(u32 *)(p + 0x9c) = resource;
    sub_08217DE0(e, resource, 1);
    Effect_SetOrigin(e, -8, -8);
    SpriteDrawable_SelectTileFrame(e, *(u32 *)(p + 0x9c), 10);
    *(struct Pair339 *)(e + 0x18) = *(struct Pair339 *)(p + 0x18);
    *(u16 *)(e + 0x1a) += 215;
    e[0xf] = 1;
    e[0x10] = 20;
}
