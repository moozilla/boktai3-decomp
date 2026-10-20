#include "global.h"
#ifdef NONMATCHING
// fuzzy: 52.3%; open: agbcc CSEs the 0x4A8 address into callee-saved r6 (orig recomputes it for the final call)

extern u8 *gUnk_020004B4;

s32 Div(s32, s32);
void Camera_SnapToWithoutCompletion(void *);

struct Q {
    u8 pad0[0x2C];
    u8 id;
    u8 pad2D[3];
    s16 x, y, z;
    u8 pad36[0x4A8 - 0x36];
    s16 cx, cy, cz;
    u8 pad4AE[2];
    s16 tx, ty, tz;
};

void sub_0823B7A4(struct Q *p)
{
    s32 other;
    p->cx = Div(p->x + p->tx + p->cx * 4, 5);
    p->cy = Div(p->y + p->ty + (p->cy * 4 + 0x96), 5);
    p->cz = Div(p->z + p->tz + p->cz * 4, 5);
    if (gUnk_020004B4 == 0)
        other = -1;
    else
        other = *(s16 *)(gUnk_020004B4 + 0x18);
    if (p->id == other)
        Camera_SnapToWithoutCompletion(&p->cx);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823B7A4);
#endif
