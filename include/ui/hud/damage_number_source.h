#ifndef GUARD_DAMAGE_NUMBER_SOURCE_H
#define GUARD_DAMAGE_NUMBER_SOURCE_H

#include "global.h"

/* Show combines this identity halfword with the source address into a key. */
struct DamageNumberSource {
    u32 unknown0;
    u16 id;
};

#endif
