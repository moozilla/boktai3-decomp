#ifndef GUARD_INCLUDE_BOSSES_NIDHOGGR_NIDHOGGR_EFFECT_MOTION_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_NIDHOGGR_NIDHOGGR_EFFECT_MOTION_LAYOUT_H

#include "global.h"

struct NidhoggrEffectMotion {
    u16 x; /* confidence=low */
    u16 y; /* confidence=low */
    u16 z; /* confidence=low */
    u16 unk_6; /* confidence=low */
} __attribute__((aligned(4)));

typedef char NidhoggrEffectMotionSizeCheck[sizeof(struct NidhoggrEffectMotion) == 0x8 ? 1 : -1];

#endif
