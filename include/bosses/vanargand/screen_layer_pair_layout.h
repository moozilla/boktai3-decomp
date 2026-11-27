#ifndef GUARD_INCLUDE_BOSSES_VANARGAND_SCREEN_LAYER_PAIR_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_VANARGAND_SCREEN_LAYER_PAIR_LAYOUT_H

#include "global.h"

struct ScreenLayerPair {
    u8 pad0[0x2c0];
    u32 firstFlags;
    u8 pad2C4[0x44];
    u32 secondFlags;
    u8 pad30C[0x2e2];
    s16 layer;
};

typedef char ScreenLayerPairSizeCheck[sizeof(struct ScreenLayerPair) == 0x5f0 ? 1 : -1];

#endif
