#ifndef GUARD_INCLUDE_BOSSES_NIDHOGGR_EFFECT_DELTA_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_NIDHOGGR_EFFECT_DELTA_LAYOUT_H

#include "global.h"

struct EffectDelta {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    u8 pad6[0x2];
};

typedef char EffectDeltaSizeCheck[sizeof(struct EffectDelta) == 0x8 ? 1 : -1];

#endif
