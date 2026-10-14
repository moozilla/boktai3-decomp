#ifndef GUARD_CONTACT_ACTOR_H
#define GUARD_CONTACT_ACTOR_H
/* Tentative sixteen-slot contact actor pool, beginning at 080427FC. */
#include "engine.h"
struct ContactPackedVector {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
};
struct ContactCollider {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x1C];
    s16 x, y, z;
    u8 unknown2A[0xE];
    u32 reactionFlags;
    u16 unknown3C, cost;
    u8 unknown40[2];
    u8 direction;
    u8 unknown43[0x11];
};
struct ContactActor {
    u8 active, state;
    u8 unknown2[2];
    u16 stateFrame;
    u8 direction, unknown7;
    s8 reactionFlag;
    u8 reactionTimer;
    u8 unknownA[0xA];
    s32 position[3];
    u8 unknown20[0xC];
    struct ContactCollider resourceCollider, healthCollider;
    u8 sequenceD4[0x10];
    u8 sprite[0x2C];
    u8 graphics[0x1C];
    u8 shadow[0x40];
};
/* Known prefix only; the complete task allocation and tile buffer length are unconfirmed. */
struct ContactActorPool {
    u8 task[0x18];
    u32 state;
    u8 unknown1C[4];
    u16 resourceColliderId, healthColliderId;
    u8 unknown24[0xC];
    u8 scriptState[0xC];
    struct ContactActor slots[16];
    u8 unknown16FC[4];
    u8 tileData[1];
};
#endif
