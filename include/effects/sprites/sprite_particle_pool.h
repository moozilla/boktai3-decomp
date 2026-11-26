#ifndef GUARD_SPRITE_PARTICLE_POOL_H
#define GUARD_SPRITE_PARTICLE_POOL_H

#include "effects/projectiles/contact_swarm_sprite.h"
#include "engine/video/display_sequence.h"
#include "engine/task.h"

struct SpriteParticleSlot {
    struct ContactPosition position;
    u8 unknown8[2];
    u8 active;
    s8 index;
};

struct SpriteParticlePool {
    struct Task task;
    void *sequenceResource;
    struct ContactSwarmSprite sprites[8];
    u8 spriteData[0x1C];
    struct DisplaySequenceState animations[8];
    u32 occupied;
    struct SpriteParticleSlot slots[8];
};

#endif
