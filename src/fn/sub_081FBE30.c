#include "global.h"
#include "ratatosk_particle.h"
#include "ram.h"

#define gRandomTableIndex gEffectRandomTableIndex

extern const u16 gUnk_086141BC[];
s32 Mod(s32, s32);

/* Tentative: accelerates a falling particle and randomizes its planar motion. */
s32 Ratatosk_UpdateParticleFallAndWander(struct RatatoskParticle *particle, s32 slot)
{
    struct RatatoskParticleConfig *params = &particle->config;
    particle->fallSpeed += particle->config.fallAcceleration;
    if (particle->fallSpeed > 0x2000)
        particle->fallSpeed = 0x2000;
    particle->fallDistance += particle->fallSpeed;
    {
        u32 one = 1;
        particle->motionFlags |= one;
    }
    if (Mod(particle->age, 20) == 0) {
        if (--params->speed <= 0)
            params->speed = 0;
        if (params->speed <= 1) {
            gRandomTableIndex = (gRandomTableIndex + 1) & 0x3FF;
            {
                u32 sample = gUnk_086141BC[gRandomTableIndex];
                u32 mask = 3;
                params->speed = sample & mask;
            }
            gRandomTableIndex = (gRandomTableIndex + 1) & 0x3FF;
            params->angle = gUnk_086141BC[gRandomTableIndex];
        }
    }
    return 0;
}
