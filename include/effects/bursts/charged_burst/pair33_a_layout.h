#ifndef GUARD_INCLUDE_EFFECTS_BURSTS_CHARGED_BURST_PAIR33_A_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_BURSTS_CHARGED_BURST_PAIR33_A_LAYOUT_H

#include "global.h"

struct Pair33A {
    u32 a;
    u32 b;
};

typedef char Pair33ASizeCheck[sizeof(struct Pair33A) == 0x8 ? 1 : -1];

#endif
