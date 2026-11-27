#ifndef GUARD_INCLUDE_EFFECTS_SUN_DISCS_DISC_POOL_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_SUN_DISCS_DISC_POOL_LAYOUT_H

#include "global.h"

struct DiscPool {
    u8 pad0[0x38];
    u32 allocated;
};

typedef char DiscPoolSizeCheck[sizeof(struct DiscPool) == 0x3c ? 1 : -1];

#endif
