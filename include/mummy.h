#ifndef GUARD_MUMMY_H
#define GUARD_MUMMY_H

#include "chase_actor.h"

/* Mummy's class fields past the chase actor, as far as its code reads them (provisional). */
struct Mummy {
    struct ChaseActor chase;
    u8 unknown624[0xB0];
    u16 flags6D4;
    u8 unknown6D6[0x12];
    u16 field6E8;
};

#endif // GUARD_MUMMY_H
