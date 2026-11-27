#ifndef GUARD_INCLUDE_EFFECTS_ORBITS_SCREEN_ORBIT_EFFECT_ORBIT_VEC_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_ORBITS_SCREEN_ORBIT_EFFECT_ORBIT_VEC_LAYOUT_H

#include "global.h"

struct OrbitVec {
    s16 x;
    s16 y;
    s16 z;
    s16 pad; /* confidence=low */
} __attribute__((aligned(4)));

typedef char OrbitVecSizeCheck[sizeof(struct OrbitVec) == 0x8 ? 1 : -1];

#endif
