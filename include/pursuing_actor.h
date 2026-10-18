#ifndef FOUNDRY_PURSUING_ACTOR_H
#define FOUNDRY_PURSUING_ACTOR_H
#include "global.h"
#include "scene_vec.h"
/* Tentative family: sprite actors with a paired collider and player homing. */
struct PursuingActorCollider {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x4c];
};
struct PursuingActorTrail {
    u8 unknown0[4];
    u8 active;
    u8 unknown5[0x57];
};
struct PursuingActor {
    u8 unknown0[6];
    u8 phase;
    u8 unknown7[0x15];
    struct SceneVec position;
    u8 unknown24[0x18];
    struct PursuingActorTrail trail[2];
    struct PursuingActorCollider primary, secondary;
    struct SceneVec velocity;
    u8 unknown1a4;
    u8 heading, turnTimer, turnSpeed;
    u8 unknown1a8[0xe];
    u8 active;
    s8 slot;
    u16 timer;
    u8 entered, turnDelay;
};
struct PursuingActorManager {
    u8 unknown0[0x38];
    u32 activeSlots;
    struct PursuingActor actors[8];
};
struct ActorColliderExtent {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
#endif
