#ifndef GUARD_PARTICLE_GROUP_H
#define GUARD_PARTICLE_GROUP_H

#include "scene/actor/contact_position.h"
#include "engine/task.h"

/* The six-group pool created by ParticleManager_Create. */
struct ParticleGroupParticle {
    u8 active, unknown1, timer, lifetime;
    u16 cell;
    s16 height;
    struct ContactPosition velocity;
    u32 flags;
    u8 unknown14[4];
    u16 tile;
    u8 unknown1A[4], palette, priority, unknown20[8];
    struct ContactPosition position;
    u8 unknown30[8];
};

struct ParticleGroup {
    u8 active, count, type, priority;
    struct ParticleGroupParticle particles[8];
};

struct ParticleGroupGraphics {
    u32 reserved;
    u16 tileBase, paletteBank;
};

struct ParticleGroupManager {
    struct Task task;
    u32 timer;
    struct ParticleGroupGraphics *small, *large;
    struct ParticleGroup groups[6];
};

#endif
