#ifndef GUARD_GOLEM_H
#define GUARD_GOLEM_H

#include "chase_actor.h"

/* Golem's class fields past the chase actor, as far as its code reads them (provisional). */
struct Golem {
    struct ChaseActor chase;
    u8 responseCount;
    u8 flag625;
    u8 unknown626[2];
    u32 flags628;
    u32 rangeSquared62C;
    u8 unknown630[4];
    u16 pendingEvent;
    u8 unknown636[2];
    struct ChaseRouteProbe routeProbe;
    struct ContactPosition destination;
    u8 unknown664[0x1C];
    s16 routeDelay;
    s16 pushSpeed;
    u16 field684;
    u16 counter686;
    u16 charge688;
    u16 field68A;
    u8 field68C;
    u8 field68D;
    u8 direction68E;
    u8 unknown68F;
    u8 savedDirection690;
    u8 direction691;
    u8 unknown692;
    u8 field693;
    struct ContactPosition home694;
    struct ContactPosition target69C;
    u8 unknown6A4[8];
    u16 shakeStrength;
    u16 shakeTimer;
};

#endif // GUARD_GOLEM_H
