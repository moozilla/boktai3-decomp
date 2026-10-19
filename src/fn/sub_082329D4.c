#include "global.h"
struct S {
    u8 a, b, c, p3;
    u32 f4;
    u8 p8[0x30 - 8];
    s16 h30, h32, h34;
};
void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void ScreenOrbitEffect_InitSlot(u8 *a, struct S *s)
{
    u32 zero = 0;
    u8 one;
    void *p;
    s->a = zero;
    s->f4 = zero;
    one = 1;
    s->c = one;
    s->b = zero;
    p = s->p8;
    a += 0x100;
    ParticleSprite_Initialize(p, *(void **)a, 0x13);
    Effect_SetOrigin(p, -4, -4);
    SpriteDrawable_SelectTileFrame(p, *(void **)a, 0x33);
    ((u8 *)s)[0x17] = one;
    s->h30 = zero;
    s->h32 = zero;
    s->h34 = zero;
}
