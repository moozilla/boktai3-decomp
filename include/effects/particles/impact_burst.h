#ifndef GUARD_IMPACT_BURST_H
#define GUARD_IMPACT_BURST_H

#include "global.h"

struct AnimatedParticleConfig {
    u16 type, flags, unused, speed;
    s16 vertical, verticalRange, horizontal, horizontalRange;
    u16 pitch, pitchRange, yaw, yawRange;
    u16 verticalStart, verticalStartRange, verticalDecay, verticalDecayRange;
    u16 horizontalStart, horizontalStartRange, horizontalDecay, horizontalDecayRange;
};

typedef char AnimatedParticleConfigSizeCheck[sizeof(struct AnimatedParticleConfig) == 0x28 ? 1 : -1];

#endif
