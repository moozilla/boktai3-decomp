#ifndef GUARD_ENEMY_PARTICLE_H
#define GUARD_ENEMY_PARTICLE_H

#include "scene/collision/contact_swarm.h"

struct EnemyParticleParams {
    u16 spriteFlags, lifetime;
    u8 unknown4[2];
    s8 angle, mode;
    u32 speed, flags;
    void *callback;
    u32 resource, frame;
    s32 bank, gravity, verticalSpeed, height;
    s16 groundHeight, shadow;
    u32 script, sequence;
};

struct EnemyParticle {
    u32 timer, flags;
    u8 unknown8[8];
    struct ContactPosition velocity;
    struct DisplaySequenceState animation;
    struct EnemyParticleParams params;
    u8 graphics[0x1C];
    struct ContactSwarmSprite sprite;
    u8 unknownA8[4];
    u32 scriptProc;
};

struct EnemyParticlePool {
    struct Task task;
    u32 mask;
    struct EnemyParticle slots[16];
};

typedef char EnemyParticleParamsSizeCheck[sizeof(struct EnemyParticleParams) == 0x38 ? 1 : -1];
typedef char EnemyParticleSizeCheck[sizeof(struct EnemyParticle) == 0xB0 ? 1 : -1];
typedef char EnemyParticlePoolSizeCheck[sizeof(struct EnemyParticlePool) == 0xB1C ? 1 : -1];

#endif
