#ifndef GUARD_INCLUDE_OBJECTS_SOLAR_ROOTS_GROWTH_HAZARD_POSITION_LAYOUT_H
#define GUARD_INCLUDE_OBJECTS_SOLAR_ROOTS_GROWTH_HAZARD_POSITION_LAYOUT_H

#include "global.h"

struct GrowthHazardPosition {
    s16 x;
    s16 y;
    s16 z;
    s16 unk6; /* confidence=low */
} __attribute__((aligned(4)));

typedef char GrowthHazardPositionSizeCheck[sizeof(struct GrowthHazardPosition) == 0x8 ? 1 : -1];

#endif
