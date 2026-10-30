#ifndef GUARD_CONTACT_ACTOR_H
#define GUARD_CONTACT_ACTOR_H
/* Tentative sixteen-slot contact actor pool, beginning at 080427FC. */
#include "display_animation.h"
#include "contact_position.h"
#include "contact_collider.h"
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
/* Task allocation is 0x1720 bytes; the final 32 bytes hold the blend buffer. */
struct ContactActorPool {
    u8 task[0x18];
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
