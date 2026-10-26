#include "global.h"
#include "ram.h"

#define gRandomTableIndex gEffectRandomTableIndex
#define gRandomTable      gUnk_086141BC

struct RatatoskParticleParams {
    u8 unknown0[6];
    s8 direction;
    u8 unknown7;
    s32 speed;
    s32 fallAcceleration;
    u8 unknown10[8];
};

struct RatatoskParticle {
    s32 frame;
    u8 unknown4[0x10];
    struct RatatoskParticleParams params;
    u8 unknown2C[0x30];
    u32 flags;
    s32 fallDistance;
    s32 fallSpeed;
};

extern const u16 gUnk_086141BC[];
s32 Mod(s32, s32);

/* Tentative: accelerates a falling particle and randomizes its planar motion. */
s32 Ratatosk_UpdateParticleFallAndWander(struct RatatoskParticle *particle)
{
    struct RatatoskParticleParams *params = &particle->params;
    particle->fallSpeed += particle->params.fallAcceleration;
    if (particle->fallSpeed > 0x2000)
        particle->fallSpeed = 0x2000;
    particle->fallDistance += particle->fallSpeed;
    {
        u32 one = 1;
        particle->flags |= one;
    }
    if (Mod(particle->frame, 20) == 0) {
        if (--params->speed <= 0)
            params->speed = 0;
        if (params->speed <= 1) {
            gRandomTableIndex = (gRandomTableIndex + 1) & 0x3FF;
            {
                u32 sample = gRandomTable[gRandomTableIndex];
                u32 mask = 3;
                params->speed = sample & mask;
            }
            gRandomTableIndex = (gRandomTableIndex + 1) & 0x3FF;
            params->direction = gRandomTable[gRandomTableIndex];
        }
    }
    return 0;
}
