#ifndef GUARD_ANIMATED_PARTICLE_H
#define GUARD_ANIMATED_PARTICLE_H

#include "engine/shared/display_sequence.h"
#include "engine/interrupts_and_video/sprite.h"
#include "engine/shared/task.h"

struct AnimatedParticle {
    u8 active, type;
    u16 timer;
    s16 verticalScale, horizontalScale;
    u16 verticalStart, verticalDecay, horizontalStart, horizontalDecay;
    struct ContactPosition direction, velocity;
    struct SpriteRenderResource resource;
    struct DisplaySequenceSprite sprite;
    struct DisplaySequenceState anim;
};
struct AnimatedParticleManager {
    struct Task task;
    void *resources[2];
    u32 timer;
    struct AnimatedParticle pieces[32];
};
#endif
