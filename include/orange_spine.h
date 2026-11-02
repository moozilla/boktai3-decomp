#ifndef GUARD_ORANGE_SPINE_H
#define GUARD_ORANGE_SPINE_H

#include "display_animation.h"
#include "contact_collider.h"
#include "scene_vec.h"

union OrangeSpineSprite {
    struct DisplaySequenceSprite display;
    struct {
        u8 unknown0[0x1C];
        struct SceneVec position;
        u8 unknown24[8];
    } coordinates;
};
struct OrangeSpineActor {
    u8 unknown0[0x18];
    u32 age;
    struct SceneVec initialPosition;
    union OrangeSpineSprite sprite;
    u8 graphics[0x1c];
    struct ContactCollider collider;
    u8 unknownC0[0x108 - 0xc0];
    void *update;
    struct DisplaySequenceState sequence;
    u8 unknown11C[0xA];
    s16 health;
    s16 damage;
    s16 hitReaction;
    u8 unknown12C[0x2];
    s16 defeated;
    s16 flashTimer;
    u8 facing;
    u8 unknown133[0x5];
    s16 maxHealth;
    u8 unknown13A[2];
    u16 bodyReactionFlags;
    u16 sparkMask;
    s16 hudStyle;
    u8 unknown142[0x6];
    s16 hudTime;
    u8 unknown14A[0x2];
    void *hud;
    u32 reactionFlags;
    u32 pendingReaction;
    u8 unknown158[0x18];
    void *resource;
};

void OrangeSpine_ApplyHitReaction(struct ContactCollider *source, struct ContactCollider *hit,
                                  struct OrangeSpineActor *actor);

#endif
