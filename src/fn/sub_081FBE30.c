#include "global.h"

#define Ratatosk_UpdateParticleFallAndWander sub_081FBE30
#define gRandomTableIndex gUnk_03005304
#define gRandomTable gUnk_086141BC

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

extern u32 gUnk_03005304;
extern const u16 gUnk_086141BC[];
s32 Mod(s32, s32);

/* Tentative: accelerates a falling particle and randomizes its planar motion. */
s32 sub_081FBE30(struct RatatoskParticle *particle)
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
