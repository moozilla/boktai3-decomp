#ifndef GUARD_POISON_HAND_H
#define GUARD_POISON_HAND_H

#include "engine/math/scene_vec.h"
#include "engine/task.h"
#include "engine/video/display_animation.h"
#include "scene/collision/contact_collider.h"

struct PoisonHand {
    struct Task task;
    s32 age;
    struct SceneVec initialPosition;
    struct DisplaySequenceSprite sprite;
    u8 graphics[0x1c];
    struct ContactCollider collider;
    u8 terrainBody[0x48];
    void *update;
    struct DisplaySequenceState sequence;
    s32 attackDistanceSquared;
    s16 effectX, effectY, effectZ, unknown126, countdown;
    s16 flashTimer;
    s16 spread, registerTerrain;
    s16 hitReaction;
    s16 defeated;
    s16 damage;
    s16 health;
    s16 maxHealth;
    s16 bodyReactionFlags, sparkMask;
    s16 hudState;
    s16 showTerrainBody;
    s16 hudTimer;
    s16 initialDead, unknown146, unknown148, unknown14A;
    void *hud;
    u32 reaction;
    u32 pendingReaction;
    u32 unknown158, unknown15C, unknown160, unknown164;
    void *animationResource;
    u8 terrainRegistration[0x10];
};

typedef char PoisonHandSizeCheck[sizeof(struct PoisonHand) == 0x17C ? 1 : -1];

#endif
