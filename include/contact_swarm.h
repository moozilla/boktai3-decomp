#ifndef GUARD_CONTACT_SWARM_H
#define GUARD_CONTACT_SWARM_H
/* Tentative second sixteen-slot contact pool, beginning at 08043F64. */
#include "contact_actor.h"
struct ContactSwarmSprite {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[8];
};
struct ContactSwarmSlot {
    u8 direction, speed, state, enter;
    u32 stateFrame;
    u8 advanceFrames, advanceSpeed, trackingFrames, unknownB;
    struct ContactPosition velocity;
    struct ContactCollider resourceCollider, healthCollider;
    struct DisplaySequenceState sequence;
    struct ContactSwarmSprite sprite;
    u8 graphics[0x1C];
    u8 shadow[0x40];
};
struct ContactSwarm {
    u8 task[0x18];
    u32 activeMask, frame;
    const void *sequenceResource;
    struct ContactSwarmSlot slots[16];
    u8 fadeActive;
    s8 fadeLevel;
    u8 fadeReverse, resourceParity;
    u8 palette[0x20];
    struct ContactPosition target;
};
#endif
