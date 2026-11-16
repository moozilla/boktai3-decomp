#ifndef GUARD_DOG_H
#define GUARD_DOG_H

#include "chase_actor.h"

/* Dog's class fields past the chase actor, as far as its code reads them (provisional). */
struct Dog {
    struct ChaseActor chase;
    u8 unknown624[0x38];
    u16 animationFlags65C;
    u8 unknown65E[0x1A];
    u32 flags;
    u32 speed;
    u32 alternateSpeed;
    u32 rangeSquared;
    u32 lastCell;
    u32 currentSpeed;
    u16 field690;
    u16 limitX, limitZ;
    u8 unknown696[6];
    u16 nearDistance, farDistance;
    u16 destinationCell;
    u16 field6A2;
    u16 field6A4;
    u8 field6A6, field6A7, field6A8, interval;
    u8 unknown6AA[2];
    struct ChaseRouteProbe routeProbe;
    struct ContactPosition destination;
};

#endif // GUARD_DOG_H
