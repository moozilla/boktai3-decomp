#ifndef GUARD_INCLUDE_BOSSES_RATATOSK_SCENE_DIRECTIONAL_ACTOR_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_RATATOSK_SCENE_DIRECTIONAL_ACTOR_LAYOUT_H

#include "global.h"

struct SceneDirectionalActor {
    u8 pad0[0x51];
    u8 direction;
    u8 pad52[0x56];
    u8 direction4;
    u8 direction8;
    u8 state;
    u8 previousState;
    u8 padAC[0x3];
    u8 pending;
    u8 entered;
    u8 padB1[0x13];
    u32 timer;
    u8 padC8[0x4];
    u8 requestedDirection;
    u8 padCD[0x3];
};

typedef char SceneDirectionalActorSizeCheck[sizeof(struct SceneDirectionalActor) == 0xd0 ? 1 : -1];

#endif
