#ifndef GUARD_INCLUDE_ENGINE_VIDEO_PROXIMITY_VOLUME_H
#define GUARD_INCLUDE_ENGINE_VIDEO_PROXIMITY_VOLUME_H

#include "global.h"
#include "scene/collision/contact_position.h"

struct ProximityVolume {
    u8 pad0[0x8];
    struct ContactPosition position;
    void *emitter; /* confidence=low */
    u8 probe[0x1c]; /* confidence=low */
    s16 targetX;
    u8 pad32[0x2];
    s16 targetZ;
    u8 pad36[0x2];
    s16 active;
    s16 damage;
};

typedef char ProximityVolumeSizeCheck[sizeof(struct ProximityVolume) == 0x3c ? 1 : -1];

#endif
