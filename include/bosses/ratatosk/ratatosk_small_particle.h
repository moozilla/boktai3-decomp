#ifndef GUARD_RATATOSK_SMALL_PARTICLE_H
#define GUARD_RATATOSK_SMALL_PARTICLE_H

#include "bosses/ratatosk/particle_config_layout.h"
#include "effects/projectiles/contact_swarm_sprite.h"
#include "engine/task.h"
#include "engine/video/display_sequence.h"
#include "engine/video/sprite.h"

/* The 24-slot particle pool (gParticlePool); the 32-slot animated one is in
 * ratatosk_particle.h. */
struct Particle {
    u32 age;
    u8 unknown4[8];
    struct ContactPosition velocity;
    struct ParticleConfig config;
    struct ContactSwarmSprite sprite;
    u32 motionFlags; /* 0x58: bit zero, set the velocity from the config on the next update */
    s32 fallDistance, fallSpeed;
    struct DisplaySequenceState sequence;
};

struct ParticlePool {
    struct Task task;
    u32 mask;
    struct SpriteRenderResource graphics;
    struct Particle particles[24];
    void *sequenceResource;
    u32 activeCount;
};

typedef char RatatoskSmallParticleSizeCheck[sizeof(struct Particle) == 0x74 ? 1 : -1];
typedef char RatatoskSmallParticlePoolSizeCheck[sizeof(struct ParticlePool) == 0xB20 ? 1 : -1];

#endif
