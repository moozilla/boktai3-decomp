#ifndef GUARD_SCENE_NPC_TRANSITION_H
#define GUARD_SCENE_NPC_TRANSITION_H

#include "engine/task.h"
#include "engine/video/sprite.h"
#include "scene/scene_animation.h"

struct TransitionMotion {
    struct ContactPosition position;
    u8 unknown8[8];
    s16 active, angle, verticalVelocity;
    u8 age, lifetime;
};
struct TransitionParticle {
    u32 flags;
    u8 unknown4[0x14];
    struct TransitionMotion motion;
};
struct TransitionEffect {
    struct Task task;
    struct DisplaySequenceSprite sprite;
    struct SceneAnimation animation;
    struct ContactPosition origin;
    s32 frame;
    u32 state;
    s32 stateFrame;
    u32 enter;
    void *graphics;
    struct TransitionParticle particles[8];
};
typedef char TransitionEffectSizeCheck[sizeof(struct TransitionEffect) == 0x1FC ? 1 : -1];
void Otenko_SpawnTransitionParticle(struct TransitionEffect *effect, s32 angle);
void Otenko_UpdateTransitionParticles(struct TransitionEffect *effect);

#endif
