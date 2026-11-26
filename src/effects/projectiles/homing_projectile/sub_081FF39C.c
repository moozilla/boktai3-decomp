#include "global.h"

/* Tentative projectile layout, observed by its primary collision callback. */
struct HomingProjectile {
    u32 lifetime;
    u8 pad04[0x78 - 4];
    struct {
        u8 unknown[6];
        u16 flags;
    } collider;
    u8 pad80[0xe8 - 0x80];
    void *animationData;
    u8 padec[6];
    s8 angle;
    u8 variant;
    u8 padf4[0x13c - 0xf4];
    s32 state;
    u32 animation;
    u8 animationFlags;
    u8 pad145[0x150 - 0x145];
    u32 lifetimeScale;
    u8 pad154[8];
    u16 impactTimer;
};
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);

void HomingProjectile_BeginImpact(void *a, void *b, struct HomingProjectile *p)
{
    u8 *params = (u8 *)p + 0xec;
    s32 sequence;
    u8 direction;
    u8 flips[2];
    u8 *first;
    u8 *second;
    u8 *saved;
    u32 check;
    struct {
        u8 unknown[6];
        u16 flags;
    } *collider;
    p->state = 3;
    p->lifetime = p->lifetimeScale * 30 + 170;
    collider = (void *)&p->collider;
    collider->flags |= 4;
    sequence = params[7] == 0 ? 2 : 3;
    direction = (((u16) * (s8 *)(params + 6) + 0x60) >> 6) & 3;
    first = &flips[0];
    second = &flips[1];
    check = (u8)(direction - 1);
    saved = second;
    if (check <= 1)
        *first = 1;
    else
        *first = 0;
    if (direction > 1)
        *second = 1;
    else
        *second = 0;
    Anim_SetSequence(&p->animation, p->animationData, (u16)sequence, flips[0], *saved);
    p->animationFlags |= 8;
    Sound_PlaySong(0x56f);
    p->impactTimer = 16;
}
