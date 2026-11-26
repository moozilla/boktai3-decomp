#ifndef GUARD_SPIDER_H
#define GUARD_SPIDER_H

#include "engine/shared/chase_actor.h"

/* Spider's class fields past the chase actor (Spider_Create allocates 0x65C bytes; provisional). */
struct Spider {
    struct ChaseActor chase;
    u8 unknown624[4];
    s16 wanderTimer, recoveryDelay;
    u16 flags62C;
    u16 timer62E;
    struct ChaseRouteProbe routeProbe;
    struct ContactPosition destination;
};

#endif // GUARD_SPIDER_H
