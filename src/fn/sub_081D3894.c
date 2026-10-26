#include "global.h"
#include "engine.h"

struct HomingSprite {
    u8 pad0[0x18];
    s16 x, y, z;
};
struct HomingTarget {
    u8 pad0[2];
    s16 y;
};
struct HomingParticle {
    u8 state;
    u8 pad1[2];
    u8 speed;
    u8 angle;
    u8 pad5[7];
    s16 vx;
    u8 padE[2];
    s16 vz;
    u8 pad12[0xA];
    struct HomingTarget *target;
    struct HomingSprite sprite_at20;
};

extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);

void Particle_UpdateHoming(u8 *actor, struct HomingParticle *p)
{
    struct HomingTarget *target = p->target;
    struct HomingSprite *sprite = &p->sprite_at20;
    s32 dx = *(s16 *)(actor + 0x24) - sprite->x;
    s32 dz = *(s16 *)(actor + 0x28) - sprite->z;
    s32 vx;

    p->angle = Math_Atan2(dx, dz);
    vx = (gUnk_086149C4[(p->angle + 0x40) & 0xFF] * p->speed) >> 12;
    p->vx = vx;
    p->vz = (gUnk_086149C4[p->angle] * p->speed) >> 12;
    if (dx * dx + dz * dz <= 99) {
        p->state = 1;
        p->speed = 0;
        gRandomIndex = (gRandomIndex + 1) & 0x3FF;
        p->angle = ((const u16 *)0x0203B400)[gRandomIndex];
    } else {
        s32 height;
        sprite->x += vx;
        height = target->y - sprite->y;
        if (height > 8)
            sprite->y += 8;
        else if (height < 8)
            sprite->y -= 8;
        else
            sprite->y = target->y;
        sprite->z += p->vz;
    }
}
