#ifndef GUARD_DOG_H
#define GUARD_DOG_H

#include "chase_actor.h"

/* Dog's class fields past the chase actor, as far as its code reads them (provisional). */
struct Dog {
    struct ChaseActor chase;
    struct ContactCollider collider; /* the bite: Collider_SetAttackProperties arms it */
    u32 flags;
    u32 speed;
    u32 alternateSpeed;
    u32 rangeSquared;
    u32 lastCell;
    u32 currentSpeed;
    u16 timer690;
    u16 limitX, limitZ;
    u8 unknown696[2];
    u16 nearTimer;
    u16 farTimer;
    u16 nearDistance, farDistance;
    u16 destinationCell;
    u16 field6A2;
    u16 timer6A4;
    u8 state6A6, emitted, turnDelay, interval;
    u8 unknown6AA[2];
    struct ChaseRouteProbe routeProbe;
    struct ContactPosition destination;
    u8 slots[8];
};

#endif // GUARD_DOG_H
