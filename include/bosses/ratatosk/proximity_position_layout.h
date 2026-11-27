#ifndef GUARD_INCLUDE_BOSSES_RATATOSK_PROXIMITY_POSITION_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_RATATOSK_PROXIMITY_POSITION_LAYOUT_H

#include "global.h"

struct ProximityPosition {
    s16 x;
    s16 y;
    s16 z;
    s16 pad; /* confidence=low */
} __attribute__((aligned(4)));

typedef char ProximityPositionSizeCheck[sizeof(struct ProximityPosition) == 0x8 ? 1 : -1];

#endif
