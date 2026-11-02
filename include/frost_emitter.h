#ifndef GUARD_FROST_EMITTER_H
#define GUARD_FROST_EMITTER_H

#include "contact_swarm_sprite.h"
#include "display_sequence.h"

struct FrostParticle {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    struct ContactPosition velocity;
    u16 lifetime, unknown46;
};
struct FrostEmitterStatus {
    u32 enabled, age;
};

struct FrostEmitter {
    u8 unknown0[0x18];
    u8 graphics[0x1C];
    void *resource;
    struct ContactPosition *source;
    u32 emitting;
    struct FrostParticle particles[12];
    struct FrostEmitterStatus runtime;
};

#endif
