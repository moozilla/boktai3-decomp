#ifndef GUARD_CONTACT_ACTOR_H
#define GUARD_CONTACT_ACTOR_H
/* Tentative sixteen-slot contact actor pool, beginning at 080427FC. */
#include "effects/shared_display_animation_wrapper/display_animation.h"
#include "engine/shared/contact_position.h"
#include "engine/shared/contact_collider.h"
#include "engine/shared/task.h"
struct ContactPackedVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
struct ContactActor {
    u8 enter, state;
    u8 finished, bobPhase;
    u16 stateFrame;
    u8 direction, steeringTimer;
    s8 targetIndex;
    u8 retargetTimer;
    u8 unknownA[2];
    s16 targetHeight, desiredHeight;
    u16 speed, unknown12;
    struct ContactPosition position;
    struct ContactPosition destination, origin;
    struct ContactCollider resourceCollider, healthCollider;
    struct DisplaySequenceState sequence;
    struct DisplaySequenceSprite sprite;
    u8 graphics[0x1C];
    u8 shadow[0x40];
};

static inline void ContactActor_EnterState(struct ContactActor *slot, u32 state)
{
    slot->state = state;
    slot->enter = 1;
    slot->stateFrame = 0;
}
/* Task allocation is 0x1720 bytes; the final 32 bytes hold the blend buffer. */
struct ContactActorPool {
    struct Task task;
    u32 state;
    u8 unknown1C, hideRequested;
    u8 clearRequested, mode;
    u16 resourceColliderId, healthColliderId;
    u32 lifetime;
    const struct ContactPosition *targets[2];
    struct ContactPosition retreatCenter;
    const void *sequenceResource;
    struct ContactActor slots[16];
    s8 fadeMode, fadeLevel;
    u8 unknown16FE[2];
    u8 tileData[0x20];
};
#endif
