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
    void *p;
    s->a = 0u;
    s->f4 = 0u;
    s->c = 1;
    s->b = 0u;
    p = s->p8;
    a += 0x100;
    ParticleSprite_Initialize(p, *(void **)a, 0x13);
    Effect_SetOrigin(p, -4, -4);
    SpriteDrawable_SelectTileFrame(p, *(void **)a, 0x33);
    ((u8 *)s)[0x17] = 1;
    s->h30 = 0u;
    s->h32 = 0u;
    s->h34 = 0u;
}
