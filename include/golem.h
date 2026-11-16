#ifndef GUARD_GOLEM_H
#define GUARD_GOLEM_H

#include "chase_actor.h"

/* Golem's class fields past the chase actor, as far as its code reads them (provisional). */
struct Golem {
    struct ChaseActor chase;
    u8 unknown624[0x5E];
    s16 field682;
    u8 unknown684[4];
    void *field688;
    u8 unknown68C;
    u8 unknown68D;
    u8 direction68E;
    u8 unknown68F;
    u8 savedDirection690;
};

#endif // GUARD_GOLEM_H
