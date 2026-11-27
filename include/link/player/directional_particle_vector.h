#ifndef GUARD_INCLUDE_LINK_PLAYER_DIRECTIONAL_PARTICLE_VECTOR_H
#define GUARD_INCLUDE_LINK_PLAYER_DIRECTIONAL_PARTICLE_VECTOR_H

#include "global.h"

struct DirectionalParticleVector {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 pad : 16; /* confidence=low */
};

typedef char DirectionalParticleVectorSizeCheck[sizeof(struct DirectionalParticleVector) == 0x8 ? 1 : -1];

#endif
