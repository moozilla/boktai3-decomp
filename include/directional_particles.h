#ifndef GUARD_DIRECTIONAL_PARTICLES_H
#define GUARD_DIRECTIONAL_PARTICLES_H
#include "contact_actor.h"
struct DirectionalParticleMotion {
    struct ContactPosition velocity;
    u32 lifetime;
};
struct DirectionalParticle {
    u8 unknown0[8];
    u32 age, flags;
    u8 unknown10[3], visible, unknown14[0x14];
    struct ContactPosition position;
    u8 unknown30[0x3C];
    struct DirectionalParticleMotion motion;
};
struct DirectionalParticleSource {
    u8 unknown0[0x24];
    const struct ContactPosition *position;
};
extern const s16 gUnk_086149C4[];
struct DirectionalParticle *SpritePool_Allocate(void);
void SpritePool_SetResource(void *, u32);
void SpritePool_SetFrame(void *, u32);
void SpritePool_SetPalette(void *, u32);
void SpritePool_SetPosition(void *, const struct ContactPosition *);
u32 SpritePool_Activate(void *, void *, void *);
#endif
