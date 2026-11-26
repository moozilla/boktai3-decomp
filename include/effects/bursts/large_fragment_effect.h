#ifndef GUARD_LARGE_FRAGMENT_EFFECT_H
#define GUARD_LARGE_FRAGMENT_EFFECT_H

#include "global.h"

struct LargeFragmentSlot {
    u8 drawable[0x1C];
    u8 pulsePosition[0x7A];
    u8 state;
    s8 index;
    u16 duration;
    u8 padding9A[2];
    u16 timer;
    u8 padding9E;
    u8 reset;
};

struct LargeFragmentSlotPool {
    u8 taskAndResource[0x38];
    u32 occupied;
    struct LargeFragmentSlot slots[8];
};

#endif
