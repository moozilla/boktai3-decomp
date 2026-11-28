#ifndef GUARD_IVY_H
#define GUARD_IVY_H

#include "monsters/chase_actor.h"

/* Bits of Ivy.captureFlags. */
enum IvyCaptureFlag {
    IVY_CAPTURE_RELEASED = 1,     /* the capture is over: contacts are ignored */
    IVY_CAPTURE_HOLDS_PLAYER = 2, /* a grabbed player still has to be put down */
};

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
