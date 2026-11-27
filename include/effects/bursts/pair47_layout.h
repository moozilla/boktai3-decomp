#ifndef GUARD_INCLUDE_EFFECTS_BURSTS_SPIRAL_BURST_PAIR47_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_BURSTS_SPIRAL_BURST_PAIR47_LAYOUT_H

#include "global.h"

struct Pair47 {
    u32 a;
    u32 b;
};

typedef char Pair47SizeCheck[sizeof(struct Pair47) == 0x8 ? 1 : -1];

#endif
