#include "global.h"

struct Ctx {
    u8 pad0[4];
    u8 *obj4;
    u8 mode8;
    u8 pad9[0x260 - 9];
    void *f260;
    u8 pad264[0x270 - 0x264];
    void *f270;
    u8 pad274[0x2A8 - 0x274];
    u8 s2A8, s2A9, s2AA, pad2AB;
    u32 c2AC;
    u8 pad2B0, f2B1;
    u8 pad2B2[0x2C2 - 0x2B2];
    u16 f2C2;
    u8 pad2C4[0x2D4 - 0x2C4];
    u16 f2D4;
    u8 pad2D6[0x3D0 - 0x2D6];
    u8 *p3D0;
    u8 pad3D4[0x5D0 - 0x3D4];
    u16 *d5D0;
};

u8 SceneActorBehavior_AccelerateTowardDescriptor(struct Ctx *, void *, u32, u32, s32);
u8 SceneActorBehavior_RequestDescriptorTurn(struct Ctx *, void *, u32, u32);
u32 sub_0821F840(u16 *p);
void sub_08220D44(void *, u32);
void sub_08220F00(void *, u32);
s32 sub_0824923C(void *arg, void *function);

void sub_080AFD88(struct Ctx *a)
{
    u16 **pp = (u16 **)((u8 *)a + 0x3D4);
    u8 *p = a->p3D0;
    s32 r7;
    u32 r6;
    u16 *q;
    u32 m = 0xA02F;

    if (a->f2B1 != 0)
        a->f2B1 = 0;
    sub_0824923C(a, a->f270);
    r7 = sub_0824923C(a, a->f260);
    a->f2C2 = 0x20;
    if (a->f2D4 & m) {
        q = &a->f2C2;
        *q = 0x40;
    } else {
        q = &a->f2C2;
        if (*(volatile u16 *)q == 0)
            *q = 0x40;
    }
    if (a->mode8 == 0)
        sub_08220D44(a->obj4 + 0x48, *q);
    else
        sub_08220F00(a->obj4 + 0x20, *q);
    pp = (u16 **)((u8 *)pp + 0x1FC);
    r6 = sub_0821F840(*pp);
    if (r6 == 0) {
        if (SceneActorBehavior_AccelerateTowardDescriptor(a, *pp, 0x20, 0x20, r7)) {
            a->s2A8 = 3;
            a->s2A9 = r6;
            a->s2AA = r6;
            a->c2AC = r6;
            a->f2B1 = 1;
            *(u16 *)(p + 0x6D4) |= 2;
            return;
        }
    } else {
        if (SceneActorBehavior_RequestDescriptorTurn(a, *pp, 0x20, 0x20)) {
            a->s2A8 = 0;
            a->s2A9 = 0;
            a->s2AA = 0x10;
            a->c2AC = 0;
            a->f2B1 = 1;
        }
    }
    a->c2AC++;
}
