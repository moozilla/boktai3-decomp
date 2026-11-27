#ifndef GUARD_INCLUDE_EFFECTS_HEALING_FLICKER_PARTICLE_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_HEALING_FLICKER_PARTICLE_LAYOUT_H

#include "global.h"

struct FlickerParticle {
    u32 flags;
    u8 pad4[0x14];
    u16 x;
    u16 y;
    u8 pad1C[0xc];
    u16 age;
    u16 active;
    u16 velocityX;
    u16 velocityY;
    u8 pad30[0x4];
    void *resource;
    void (*callback)(struct FlickerParticle *); /* confidence=low */
};

typedef char FlickerParticleSizeCheck[sizeof(struct FlickerParticle) == 0x3c ? 1 : -1];

#endif
