#ifndef GUARD_CONVERGING_PARTICLES_H
#define GUARD_CONVERGING_PARTICLES_H
#include "global.h"

struct ConvergingParticlePoint {
    u16 x, depth, y, padding;
};
struct ConvergingParticle {
    u32 flags;
    u8 unknown4[0xB];
    u8 priority;
    u8 unknown10[8];
    struct ConvergingParticlePoint position;
    u8 unknown20[8];
    u8 active, ticks, speed, kind;
    s16 dx;
    u16 unknown2E;
    s16 dy;
    u16 unknown32;
};
struct ConvergingParticleGroup {
    void *graphics;
    u8 active, next, ticks, unknown7;
    struct ConvergingParticle particles[4];
};
struct ActorDamageSource {
    u8 unknown0[0x2D];
    u8 direction;
};
#endif
