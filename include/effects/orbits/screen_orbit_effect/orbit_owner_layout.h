#ifndef GUARD_INCLUDE_EFFECTS_ORBITS_SCREEN_ORBIT_EFFECT_ORBIT_OWNER_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_ORBITS_SCREEN_ORBIT_EFFECT_ORBIT_OWNER_LAYOUT_H

#include "global.h"
#include "effects/orbits/screen_orbit_effect/orbit_vec_layout.h"

struct OrbitOwner {
    u8 pad0[0x18];
    struct OrbitVec center;
};

typedef char OrbitOwnerSizeCheck[sizeof(struct OrbitOwner) == 0x20 ? 1 : -1];

#endif
