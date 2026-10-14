#ifndef GUARD_ORBIT_CONTACT_ACTOR_H
#define GUARD_ORBIT_CONTACT_ACTOR_H
#include "contact_swarm.h"
/* Tentative identity: rotating resource-contact sprite, list-8 task size 0xC8. */
struct OrbitContactActor {
    u8 task[0x18];
    s16 radius;
    u8 angle, state, stateFlag;
    u8 unknown1D[3];
    u32 stateFrame;
    struct ContactPosition center;
    struct ContactCollider collider;
    struct ContactSwarmGraphics graphics;
    struct ContactSwarmSprite sprite;
};
#endif
