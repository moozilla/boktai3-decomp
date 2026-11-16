#ifndef GUARD_GOLEM_H
#define GUARD_GOLEM_H

#include "chase_actor.h"

/* Golem's class fields past the chase actor, as far as its code reads them (provisional). */
struct Golem {
    struct ChaseActor chase;
    u8 field624;
    u8 unknown625[3];
    u32 field628;
    u8 unknown62C[0xC];
    struct ChaseRouteProbe routeProbe;
    struct ContactPosition destination;
    u8 unknown664[0x1C];
    u16 field680, field682, field684, field686;
    u8 unknown688[2];
    u16 field68A;
    u8 field68C;
    u8 field68D;
    u8 direction68E;
    u8 unknown68F;
    u8 savedDirection690;
    u8 unknown691[2];
    u8 field693;
};

#endif // GUARD_GOLEM_H
