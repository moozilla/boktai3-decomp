#include "global.h"
#ifdef NONMATCHING
// fuzzy: 44.3%
// fix.py best (sweep after the inlined-insn fix)
// register allocation: flags/count pointers swap (r3/r5); asrs ok
struct A {
    u8 pad0[0x9c];
    u32 flags;
    u8 padA0[0xB1 - 0xA0];
    u8 restart;
    u8 padB2[0x104 - 0xB2];
    u32 callback;
    u8 pad108[0x2AC - 0x108];
    u8 animationState[0x20];
    u8 animation[0x510 - 0x2CC];
    s8 rnd;
    u8 count;
};
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Callback_InvokeActor(void *, u32);
static inline s32 Rand16(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return gUnk_0203B400[gUnk_03005308];
}
void sub_0817E378(struct A *p)
{
    if (p->restart) {
        p->restart = 0;
        SceneActors_StartDirectionalAnimation(p, p->animation, p->animationState, 0x114, 1, 1, 4);
        p->count = 6;
    }
    if (p->count) {
        p->rnd = Rand16() >> 8;
        {
            u32 k1 = 0x20000;
            p->flags |= k1;
        }
        p->count--;
    }
    {
        u32 k2 = 0x400005;
        p->flags |= k2;
    }
    p->flags |= 0x40;
    Callback_InvokeActor(p, p->callback);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0817E378);
#endif
