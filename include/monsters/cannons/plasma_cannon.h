#ifndef GUARD_MONSTERS_CANNONS_PLASMA_CANNON_H
#define GUARD_MONSTERS_CANNONS_PLASMA_CANNON_H

#include "engine/video/display_animation.h"
#include "scene/collision/contact_collider.h"
#include "engine/math/scene_vec.h"
#include "engine/task.h"

union OrangeSpineSprite {
    struct DisplaySequenceSprite display;
    struct {
        u8 unknown0[0x1C];
        struct SceneVec position;
        u8 unknown24[8];
    } coordinates;
};
struct OrangeSpineActor {
    struct Task task;
    u32 age;
    struct SceneVec initialPosition;
    union OrangeSpineSprite sprite;
    u8 graphics[0x1c];
    struct ContactCollider collider;
    u8 unknownC0[0x108 - 0xc0];
    void *update;
    struct DisplaySequenceState sequence;
    u8 unknown11C[4];
    s16 projectileHitX, projectileHitY, projectileHitZ;
    s16 health;
    s16 damage;
    s16 hitReaction;
    u16 recoveryTimer;
    s16 defeated;
    s16 flashTimer;
    u8 facing;
    u8 unknown133;
    u16 recoveryDelay;
    s16 unknown136;
    s16 maxHealth;
    s16 projectileSpeed;
    u16 bodyReactionFlags;
    u16 sparkMask;
    s16 hudStyle;
    s16 showTerrainBody, unknown144, unknown146;
    s16 hudTime;
    s16 defeatDelay;
    void *hud;
    u32 reactionFlags;
    u32 pendingReaction;
    u32 unknown158, unknown15C, unknown160, unknown164, unknown168;
    u32 registerTerrain;
    void *resource;
    u8 terrainRegistration[0x24];
};

typedef char OrangeSpineActorSizeCheck[sizeof(struct OrangeSpineActor) == 0x198 ? 1 : -1];

void PlasmaCannon_ApplyHitReaction(struct ContactCollider *source, struct ContactCollider *hit,
                                  struct OrangeSpineActor *actor);

#endif
