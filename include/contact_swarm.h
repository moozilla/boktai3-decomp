#ifndef GUARD_CONTACT_SWARM_H
#define GUARD_CONTACT_SWARM_H
/* Tentative second sixteen-slot contact pool, beginning at 08043F64. */
#include "contact_actor.h"
#include "contact_swarm_sprite.h"
struct ContactSwarmGraphics {
    u8 unknown0[0xC];
    const void *paletteData;
    u8 unknown10[0xC];
};
struct ContactSwarmDrawPacket {
    struct ContactSwarmSprite sprite;
    struct ContactSwarmGraphics graphics;
};
struct ContactSwarmAnimation {
    struct DisplaySequenceState sequence;
    struct ContactSwarmDrawPacket draw;
};
struct ContactSwarmColliderAnimation {
    struct ContactCollider healthCollider;
    struct ContactSwarmAnimation animation;
};
struct ContactSwarmSlot {
    u8 direction, speed;
    s8 state;
    u8 enter;
    u32 stateFrame;
    u8 advanceFrames, advanceSpeed, trackingFrames, unknownB;
    struct ContactPosition velocity;
    struct ContactCollider resourceCollider;
    struct ContactSwarmColliderAnimation display;
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
