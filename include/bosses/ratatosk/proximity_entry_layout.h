#ifndef GUARD_INCLUDE_BOSSES_RATATOSK_PROXIMITY_ENTRY_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_RATATOSK_PROXIMITY_ENTRY_LAYOUT_H

#include "global.h"
#include "engine/math/scene_vec.h"

struct ProximityEntry {
    struct SceneVec position; /* confidence=low */
    s32 squaredDistance; /* confidence=low */
    s32 distanceSquared; /* confidence=low */
    u8 pad10[0x8];
};

typedef char ProximityEntrySizeCheck[sizeof(struct ProximityEntry) == 0x18 ? 1 : -1];

#endif
