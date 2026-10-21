// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct Pair {
    u32 a, b;
};
void AnimData_Load(void *, s32);
void Sprite_Init(void *, void *, s32);
void *AnimData_FindEntry(s32);
void ParticleSprite_Initialize(void *, void *, s32);
void Effect_SetOrigin(void *, s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, s32);
void Effect_SetPalette(void *, s32);
s32 SceneEffect_InitParticleRing(u8 *p, struct Pair *src)
{
    u8 *anim;
    u8 *particle;
    s32 i, neg;
    register u32 z asm("r0");
    *(struct Pair *)(p + 0x60) = *src;
    *(u32 *)(p + 0x68) = 0;
    anim = p + 0x44;
    AnimData_Load(anim, 0x3641);
    Sprite_Init(p + 0x18, anim, 2);
    *(struct Pair *)(p + 0x34) = *src;
    p[0x20] = 0x40;
    p[0x21] = 0x40;
    p[0x1f] = 3;
    *(void **)(p + 0x78) = AnimData_FindEntry(0x1C1E);
    neg = -4;
    particle = anim + 0x38;
    i = 7;
    do {
        ParticleSprite_Initialize(particle, *(void **)(p + 0x78), 1);
        Effect_SetOrigin(particle, neg, neg);
        SpriteDrawable_SelectTileFrame(particle, *(void **)(p + 0x78), 2);
        Effect_SetPalette(particle, 1);
        z = 0;
        *(u16 *)(particle + 0x28) = z;
        particle += 0x30;
    } while (--i >= 0);
    *(u32 *)(p + 0x6c) = z;
    *(u32 *)(p + 0x70) = z;
    *(u32 *)(p + 0x74) = 1;
    return 0;
}
