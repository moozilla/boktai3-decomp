#ifndef GUARD_IVY_H
#define GUARD_IVY_H

#include "monsters/chase_actor.h"

/* Ivy's class fields past the chase actor (Ivy_Create allocates 0x640 bytes; provisional). */
struct Ivy {
    struct ChaseActor chase;
    u8 unknown624[8];
    u32 captureFlags;
    u8 unknown630[4];
    s16 targetMissFrames;
    u16 field636;
    u32 field638;
    u8 startTick;
    u8 holdTicks;
    u8 interval;
    u8 unknown63F;
};

#endif // GUARD_IVY_H
