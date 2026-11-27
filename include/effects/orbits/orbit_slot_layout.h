#ifndef GUARD_INCLUDE_EFFECTS_ORBITS_SCREEN_ORBIT_EFFECT_ORBIT_SLOT_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_ORBITS_SCREEN_ORBIT_EFFECT_ORBIT_SLOT_LAYOUT_H

#include "global.h"
#include "effects/orbits/orbit_vec_layout.h"

struct OrbitSlot {
    u8 state;
    u8 angle;
    u8 enter;
    u8 unknown3; /* confidence=low */
    u32 age;
    u32 flags;
    u8 padC[0x3];
    u8 priority;
    u8 pad10[0x10];
    struct OrbitVec position;
    u8 pad28[0x8];
    struct OrbitVec world;
};

typedef char OrbitSlotSizeCheck[sizeof(struct OrbitSlot) == 0x38 ? 1 : -1];

#endif
