#ifndef GUARD_POSITIONED_EFFECT_COORDINATES_H
#define GUARD_POSITIONED_EFFECT_COORDINATES_H
#include "contact_position.h"
struct PositionedEffectCoordinates {
    u8 unknown0[8];
    struct ContactPosition position, shifted;
};
#endif
