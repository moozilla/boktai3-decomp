#include "global.h"
struct S { u8 f0[0x48]; s16 a48; s16 a4a; s16 a4c; u8 g0[0xB3A-0x4E]; u8 b3a; u8 f1; u8 b3c; u8 b3d; u8 b3e; u8 b3f; u8 b40; u8 f2[4]; u8 b45; u8 g1[0x1124-0xB46]; s16 x; s16 y; s16 z;};
s32 sub_08186C50(struct S *p)
{
    s32 dx = p->x - p->a48;
    s32 dy = p->z - p->a4c;
    s32 r;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    r = 0;
    if (dx <= 0xbf && dy <= 0xbf) r = 1;
    return r;
}
