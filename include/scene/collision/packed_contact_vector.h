#ifndef GUARD_INCLUDE_SCENE_COLLISION_PACKED_CONTACT_VECTOR_H
#define GUARD_INCLUDE_SCENE_COLLISION_PACKED_CONTACT_VECTOR_H

#include "global.h"

struct PackedContactVector {
    s32 x : 16; /* confidence=low */
    s32 y : 16; /* confidence=low */
    s32 z : 16; /* confidence=low */
    s32 pad : 16; /* confidence=low */
};

typedef char PackedContactVectorSizeCheck[sizeof(struct PackedContactVector) == 0x8 ? 1 : -1];

#endif
