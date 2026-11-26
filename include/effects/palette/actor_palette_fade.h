#ifndef GUARD_ACTOR_PALETTE_FADE_H
#define GUARD_ACTOR_PALETTE_FADE_H
#include "global.h"
struct ActorPaletteFade {
    u16 output[32];
    u16 timer, duration, targetFirst, targetSecond, sourceFirst, sourceSecond;
    s16 channels[3][32], increments[3][32];
    u32 flags;
};
#endif
