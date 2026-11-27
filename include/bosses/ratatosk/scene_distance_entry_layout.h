#ifndef GUARD_INCLUDE_BOSSES_RATATOSK_SCENE_DISTANCE_ENTRY_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_RATATOSK_SCENE_DISTANCE_ENTRY_LAYOUT_H

#include "global.h"
#include "engine/math/scene_vec.h"

struct SceneDistanceEntry {
    struct SceneVec position;
    u8 pad8[0x4];
    s32 distanceSquared;
    s32 targetDistanceSquared; /* confidence=low */
    u8 pad14[0x4];
};

typedef char SceneDistanceEntrySizeCheck[sizeof(struct SceneDistanceEntry) == 0x18 ? 1 : -1];

#endif
