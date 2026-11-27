#ifndef GUARD_INCLUDE_BOSSES_RATATOSK_PARTICLE_CONFIG_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_RATATOSK_PARTICLE_CONFIG_LAYOUT_H

#include "global.h"

struct ParticleConfig {
    u16 field0; /* confidence=low */
    u16 duration; /* confidence=low */
    u16 field4; /* confidence=low */
    s8 angle; /* confidence=low */
    u8 field7; /* confidence=low */
    u32 mode; /* confidence=low */
    u32 scale; /* confidence=low */
    u32 callback; /* confidence=low */
    u32 kind; /* confidence=low */
};

typedef char ParticleConfigSizeCheck[sizeof(struct ParticleConfig) == 0x18 ? 1 : -1];

#endif
