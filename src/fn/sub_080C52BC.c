#include "global.h"


struct SkeletonLink {
    u8 filler[0x628];
    u32 h628;
};

struct SkeletonActor {
    u8 filler0[0xDC];
    u32 h0DC;
    u8 filler1[0x2A8 - 0xE0];
    u32 f2A8;
    u8 filler2[0x2B5 - 0x2AC];
    u8 f2B5;
    u8 filler3[0x2D4 - 0x2B6];
    u16 flags;
    u8 filler4[0x3D0 - 0x2D6];
    struct SkeletonLink *t;
};

void Skeleton_ClearPendingFlag(struct SkeletonActor *s)
{
    u32 mask;

    if (s->f2B5 == 9)
        return;
    mask = 0x100;
    if (mask & s->flags)
        return;
    if ((s->f2A8 & 0x00FFFFFF) == 0x200)
        return;
    {
        u32 *p = &s->t->h628;
        u32 finalMask = 0xFFF7FFFF;
        *p = finalMask & *p;
    }
}
