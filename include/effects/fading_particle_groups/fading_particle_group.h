#ifndef GUARD_FADING_PARTICLE_GROUP_H
#define GUARD_FADING_PARTICLE_GROUP_H

#include "engine/shared/display_sequence.h"
#include "engine/sprite_oam/sprite_oam.h"
#include "engine/shared/sprite_effect_controller.h"
#include "engine/shared/task.h"

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
