#include "global.h"
struct S {
    u8 f0;
    u8 b1;
    u8 f2[5];
    u8 b7;
    u8 f8;
    u8 b9;
    u8 fa[4];
    u16 flags;
    u8 g[0x2e4 - 0x10];
    void *cb;
};
s32 Actor_PlayAnimationEntry(void *, s32, s32, s32, s32);
s32 Callback_InvokeActor(void *, void *);
s32 sub_081C8FB0(void *, void *, s32);
void StateActor_SimpleStep(void *a, struct S *p)
{
    u8 hit;
    if (p->b9 != 0) {
        p->b9 = 0;
        p->b7 = 0;
        Actor_PlayAnimationEntry(p, 0, 1, 0, 4);
    }
    {
        u32 m = 0x1000;
        p->flags = m | p->flags;
    }
    hit = 0;
    if ((u8)Callback_InvokeActor(p, p->cb) != 1 && sub_081C8FB0(a, p, 0)) {
        hit = 1;
        {
            u32 m = 0x800;
            p->flags = m | p->flags;
        }
    }
}
