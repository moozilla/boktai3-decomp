#ifndef GUARD_CONTACT_ACTOR_H
#define GUARD_CONTACT_ACTOR_H
/* Tentative sixteen-slot contact actor pool, beginning at 080427FC. */
#include "display_animation.h"
#include "contact_position.h"
struct ContactPackedVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
struct ContactCollider {
    struct ContactCollider *next; /* intrusive collider-manager list link */
    u8 unknown4[2];
    u16 flags;
    u8 unknown8[2];
    u16 contactFlags; /* 0xA: bit four enables player cost processing */
    u8 unknownC[8];
    struct ContactPosition offset, bounds;
    s16 x, y, z;
    u8 unknown2A[0xA];
    u32 statRewardFlags; /* 0x34: selects player hundredths gain on contact (tentative) */
    u32 reactionFlags;
    u16 unknown3C, cost;
    u16 responseParameter; /* 0x40: copied to actor response state (tentative) */
    u8 direction;
    u8 costFlags;      /* 0x43: bit three latches quarter-cost adjustment */
    u16 responseValue; /* 0x44: copied to actor response state (tentative) */
    u8 responseFlags;  /* 0x46: edge-triggered player contact gates (tentative) */
    u8 unknown47[9];
    void *owner; /* 0x50: callback owner */
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
