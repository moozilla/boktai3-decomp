#ifndef GUARD_INCLUDE_OBJECTS_DECK_CANNONS_CANNON_BATTERY_POSITION_LAYOUT_H
#define GUARD_INCLUDE_OBJECTS_DECK_CANNONS_CANNON_BATTERY_POSITION_LAYOUT_H

#include "global.h"

struct CannonBatteryPosition {
    s16 x;
    s16 y;
    s16 z;
    s16 unk6; /* confidence=low */
} __attribute__((aligned(4)));

typedef char CannonBatteryPositionSizeCheck[sizeof(struct CannonBatteryPosition) == 0x8 ? 1 : -1];

#endif
