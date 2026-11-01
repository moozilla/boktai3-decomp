#ifndef GUARD_SWORD_ORBIT_H
#define GUARD_SWORD_ORBIT_H

#include "global.h"

static inline s8 Sword_OrbitDirection(u32 positive)
{
    s16 direction[1] = {1};

    direction[0] = -direction[0];
    if (positive)
        direction[0] = 1;
    return direction[0];
}

#endif
