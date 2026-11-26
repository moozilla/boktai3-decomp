#ifndef GUARD_OOZE_H
#define GUARD_OOZE_H

#include "scene/collision/contact_swarm.h"

struct Ooze {
    struct Task task;
    struct ContactPosition initialPosition;
    struct DisplaySequenceSprite sprite;
    struct ContactSwarmGraphics graphics;
    struct ContactCollider collider;
    u8 body[0x48];
    void (*state)(void);
    struct DisplaySequenceState sequence;
    s16 extentX, extentY, extentZ, attackTimer;
    s16 interval, variant;
    s16 delayCount, delayIndex;
    s16 delays[8];
    s16 repeatAttack, repeatLimit, repeatCount, attackDelay, phase, dispatchTicks;
    s16 facing;
    u16 idleSequence, startSequence, holdSequence, endSequence;
    s16 solid;
    void *parent;
    u8 pad154[4];
    void *resource;
    u8 *delayScript;
};

typedef char OozeSizeCheck[sizeof(struct Ooze) == 0x160 ? 1 : -1];

#endif
