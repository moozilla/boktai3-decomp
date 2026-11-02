#ifndef GUARD_SCENE_NPC_TRANSITION_H
#define GUARD_SCENE_NPC_TRANSITION_H

#include "contact_position.h"

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
    u8 unknown0[0x60];
    struct ContactPosition origin;
    u8 unknown68[0x10];
    void *graphics;
    struct TransitionParticle particles[8];
};
void SceneNpc_SpawnTransitionParticle(struct TransitionEffect *effect, s32 angle);
void SceneNpc_UpdateTransitionParticles(struct TransitionEffect *effect);

#endif
