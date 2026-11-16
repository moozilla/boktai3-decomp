#ifndef GUARD_OCTOPUS_H
#define GUARD_OCTOPUS_H

#include "chase_actor.h"

/* Octopus's class fields past the chase actor (Octopus_Create allocates 0x63C bytes; provisional). */
struct Octopus {
    struct ChaseActor chase;
    u8 unknown624[4];
    u32 flags628;
    u32 captureFlags;
    u8 unknown630[6];
    u16 field636;
    u8 startTick;
    u8 holdTicks;
    u8 interval;
    u8 unknown63B;
};

#endif // GUARD_OCTOPUS_H
