#ifndef GUARD_INCLUDE_OBJECTS_SOLAR_ROOTS_ANIMATED_MARKER_COLLIDER_LAYOUT_H
#define GUARD_INCLUDE_OBJECTS_SOLAR_ROOTS_ANIMATED_MARKER_COLLIDER_LAYOUT_H

#include "global.h"

struct AnimatedMarkerCollider {
    u8 pad0[0x6];
    u16 flags; /* confidence=low */
    u16 pad8; /* confidence=low */
    u16 extent;
} __attribute__((aligned(4)));

typedef char AnimatedMarkerColliderSizeCheck[sizeof(struct AnimatedMarkerCollider) == 0xc ? 1 : -1];

#endif
