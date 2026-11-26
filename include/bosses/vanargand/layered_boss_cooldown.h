#ifndef GUARD_SLOT7_COOLDOWN_H
#define GUARD_SLOT7_COOLDOWN_H

#include "global.h"

struct Slot7Actor {
    u8 unknown0[0x5C0];
    u16 duration, flashTimer;
    u8 unknown5C4[0x2C];
    s16 baseDuration, contactDuration;
};

static inline u16 Slot7_IncreaseDuration(u16 frames, u32 shift)
{
    return frames + ((s16)frames >> shift);
}

#endif
