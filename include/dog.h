#ifndef GUARD_DOG_H
#define GUARD_DOG_H

#include "chase_actor.h"

/* Dog's class fields past the chase actor, as far as its code reads them (provisional). */
struct Dog {
    struct ChaseActor chase;
    u8 unknown624[0x38];
    u16 animationFlags65C;
};

#endif // GUARD_DOG_H
