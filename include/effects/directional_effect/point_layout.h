#ifndef GUARD_INCLUDE_EFFECTS_DIRECTIONAL_EFFECT_POINT_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_DIRECTIONAL_EFFECT_POINT_LAYOUT_H

#include "global.h"

struct Point {
    u16 x;
    u16 z;
    u16 y;
    u16 pad6; /* confidence=low */
} __attribute__((aligned(4)));

typedef char PointSizeCheck[sizeof(struct Point) == 0x8 ? 1 : -1];

#endif
