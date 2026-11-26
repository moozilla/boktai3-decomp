#ifndef GUARD_CONTACT_POSITION_H
#define GUARD_CONTACT_POSITION_H

#include "global.h"

struct ContactPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));

#endif
