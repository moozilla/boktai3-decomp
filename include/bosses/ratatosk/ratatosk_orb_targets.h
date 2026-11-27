#ifndef GUARD_RATATOSK_ORB_TARGETS_H
#define GUARD_RATATOSK_ORB_TARGETS_H

#include "objects/sun_orb/sun_orb_effect.h"

struct RatatoskOrbTarget {
    u32 active;
    struct OrbitEffectPosition position;
    u32 referenceDistance, playerDistance;
    u32 reserved;
};
/* The two selection getters use the last word of the fourth 0x18-byte target. */
union RatatoskLinkedOrbs {
    struct RatatoskOrbTarget targets[4];
    struct {
        u8 unknown0[0x5C];
        s16 selectedPoint, selectedTarget;
    } selection;
};

#endif
