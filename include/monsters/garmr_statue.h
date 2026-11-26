#ifndef GUARD_GARMR_STATUE_H
#define GUARD_GARMR_STATUE_H

#include "engine/math/scene_vec.h"
#include "engine/task.h"
#include "engine/video/display_animation.h"
#include "scene/collision/contact_actor.h"

union GarmrStatueSprite {
    struct DisplaySequenceSprite display;
    struct {
        u8 unknown0[0x1c];
        struct SceneVec position;
        u8 unknown24[8];
    } coordinates;
};
struct GarmrStatue {
    struct Task task;
    u32 age;
    struct SceneVec initialPosition;
    union GarmrStatueSprite sprite;
    u8 graphics[0x1c];
    struct ContactCollider collider;
    u8 terrainBody[0x48];
    void (*update)(void);
    struct DisplaySequenceState sequence;
    s32 triggerDistanceSquared;
    s16 effectA, effectB, effectC, fireTimer;
    s16 flashTimer;
    s16 projectileSpeed;
    s16 damage;
    s16 hitReaction;
    s16 defeated;
    s16 health;
    s16 activeAnimation;
    s16 maxHealth;
    s16 bodyReactionFlags, sparkMask;
    s16 hudStyle;
    s16 showTerrainBody, unknown140, unknown142;
    s16 hudTime;
    s16 defeatDelay;
    void *hud;
    u32 reactionFlags;
    u32 pendingReaction;
    u32 unknown154, unknown158, unknown15C, unknown160;
    void *resource;
};

typedef char GarmrStatueSizeCheck[sizeof(struct GarmrStatue) == 0x168 ? 1 : -1];

#endif
