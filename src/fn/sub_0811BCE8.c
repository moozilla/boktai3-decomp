#include "global.h"

struct E {
    u8 pad[0x24];
    s16 v;
    u8 pad2[0x3A];
};
struct S {
    struct E e[5];
};
extern s8 gUnk_086106C0[];

void Actor_SetSpriteDepthOffsets(struct S *p, s32 d)
{
    s32 i;
    for (i = 0; i < 3; i++)
        p->e[i].v = gUnk_086106C0[i] + d;
    p->e[3].v = gUnk_086106C0[3] + d;
    p->e[4].v = gUnk_086106C0[3] + d;
}
