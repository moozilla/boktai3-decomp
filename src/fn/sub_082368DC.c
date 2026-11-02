#include "global.h"

struct OrbitPosition {
    s16 x, y, z, pad;
};
struct OrbitSprite {
    u32 flags;
    u8 pad4[3];
    u8 f07;
    u8 pad08[8];
    u16 f10;
    u8 pad12[0xA];
    struct OrbitPosition position;
    u8 pad24[8];
};
struct OrbitSpriteGroup {
    struct OrbitSprite a;
    u8 animA[0x1C];
    struct OrbitSprite b;
    u8 animB[0x1C];
    u8 f90;
    u8 f91;
    u8 red, green, blue;
    u8 verticalStep;
    u8 pad96[2];
    void *update;
    void *owner;
};
struct OrbitEffect {
    u8 pad0[0x18];
    u8 variant;
    u8 pad19[7];
    struct OrbitSpriteGroup group;
};
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void sub_08236790(void);

void OrbitEffect_Initialize(struct OrbitEffect *p, struct OrbitPosition *pos0)
{
    struct OrbitPosition *pos = pos0;
    struct OrbitSpriteGroup *g = &p->group;
    u32 firstPalette;
    u32 secondPalette;
    void *anim;
    u32 one;
    struct OrbitSprite *s2;
    if (!p->variant) {
        firstPalette = 0;
        secondPalette = 3;
        g->red = 31;
        g->green = 31;
        g->blue = 20;
    } else {
        firstPalette = 6;
        secondPalette = 2;
        g->red = 20;
        g->green = 0;
        g->blue = 0;
    }
    anim = g->animA;
    AnimData_Load(anim, 0x210E);
    Sprite_Init(&g->a, anim, 0);
    one = 1;
    g->a.f10 = 1;
    AnimData_SelectPaletteBank(anim, firstPalette + 0x4B);
    g->a.f07 = 2;
    g->a.flags |= one;
    g->a.position = *pos0;
    s2 = &g->b;
    anim = g->animB;
    AnimData_Load(anim, 0x8639);
    Sprite_Init(s2, anim, 0);
    s2->f10 = 0;
    AnimData_SelectPaletteBank(anim, secondPalette + 0x153);
    s2->f07 = 1;
    s2->flags |= one;
    g->verticalStep = 0x24;
    g->owner = p;
    g->f90 = 0;
    g->f91 = 0;
    g->update = sub_08236790;
}
