#include "global.h"
// MATCHING_CONCESSION: register-pinning

#define OrbitEffect_Initialize sub_082368DC

struct OeVec {
    s16 x, y, z, pad;
};
struct OeSprite {
    u32 flags;
    u8 pad4[3];
    u8 f07;
    u8 pad08[8];
    u16 f10;
    u8 pad12[0xA];
    struct OeVec position;
    u8 pad24[8];
};
struct OeGroup {
    struct OeSprite a;
    u8 animA[0x1C];
    struct OeSprite b;
    u8 animB[0x1C];
    u8 f90;
    u8 f91;
    u8 pad92[3];
    u8 f95;
    u8 pad96[2];
    void *update;
    void *owner;
};
struct OeUnit {
    u8 pad0[0x18];
    u8 f18;
    u8 pad19[7];
    struct OeGroup group;
};
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void sub_08236790(void);

void OrbitEffect_Initialize(struct OeUnit *p, struct OeVec *pos0)
{
    register struct OeVec *pos asm("r8") = pos0;
    struct OeGroup *g = &p->group;
    u32 x;
    u32 y;
    u8 *q = (u8 *)p;
    void *anim;
    register u32 v asm("r0");
    u8 *t;
    u8 *w;
    u32 one;
    register u32 zero asm("sb");
    struct OeSprite *s2;
    if (p->f18 == 0) {
        x = 0;
        y = 3;
        t = q + 0xB2;
        *t = 0x1F;
        t++;
        *t = 0x1F;
        t++;
        v = 0x14;
    } else {
        x = 6;
        y = 2;
        w = q + 0xB2;
        v = 0;
        *w = 0x14;
        t = q + 0xB3;
        *t = v;
        t++;
    }
    *t = v;
    anim = g->animA;
    AnimData_Load(anim, 0x210E);
    Sprite_Init(&g->a, anim, 0);
    one = 1;
    zero = 0;
    g->a.f10 = one;
    AnimData_SelectPaletteBank(anim, x + 0x4B);
    g->a.f07 = 2;
    g->a.flags |= one;
    {
        register struct OeVec *t2 asm("r2") = pos;
        g->a.position = *t2;
    }
    s2 = &g->b;
    anim = g->animB;
    AnimData_Load(anim, 0x8639);
    Sprite_Init(s2, anim, 0);
    s2->f10 = zero;
    AnimData_SelectPaletteBank(anim, y + 0x153);
    s2->f07 = 1;
    s2->flags |= one;
    g->f95 = 0x24;
    g->owner = p;
    g->f90 = 0;
    g->f91 = 0;
    g->update = sub_08236790;
}
