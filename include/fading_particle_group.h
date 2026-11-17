#ifndef GUARD_FADING_PARTICLE_GROUP_H
#define GUARD_FADING_PARTICLE_GROUP_H

#include "display_sequence.h"
#include "sprite_oam.h"
#include "sprite_effect_controller.h"
#include "task.h"

struct FadingParticleMotion {
    struct ContactPosition velocity;
    struct SpriteDrawable sprite;
    struct DisplaySequenceState anim;
};

struct FadingParticle {
    u8 active, lifetime;
    s16 speed;
    struct FadingParticleMotion motion;
};

struct FadingParticleGroup {
    u8 active, count, unknown2[2];
    struct FadingParticle particles[8];
};

struct FadingParticleManager {
    struct Task task;
    u32 frame;
    const struct SpriteEffectTileResource *graphics;
    const void *animations;
    struct FadingParticleGroup groups[6];
};

#endif
