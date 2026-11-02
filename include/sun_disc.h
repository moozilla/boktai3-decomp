#ifndef GUARD_SUN_DISC_H
#define GUARD_SUN_DISC_H

#include "global.h"

struct SunDiscVec {
    u16 x, y, z, pad;
};
struct SunDiscDelta {
    s16 x, y, z, pad;
};
struct SunDiscSprite {
    u32 flags;
    u8 active, unknown5, unknown6, priority;
    u8 pad8[4];
    void *graphics;
    u8 pad10[12];
    struct SunDiscVec position;
    u8 pad24[8];
    u8 anim[0x58];
    struct SunDiscVec target;
    s16 velX, velY, velZ, pad92;
    u8 unknown94[0x5C];
    u16 age;
    u8 state, slot, unknownF4[4], resetPending, timer;
    u8 unknownFA[2];
};

struct SunDiscPool {
    u8 task[0x18];
    u8 graphics[0x1C];
    void *sequenceResource;
    u32 used;
    struct SunDiscSprite sprites[12];
    u32 suspended;
};

#endif
