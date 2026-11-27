#ifndef GUARD_INCLUDE_SCENE_COLLISION_PACKED_CONTACT_HALFWORDS_H
#define GUARD_INCLUDE_SCENE_COLLISION_PACKED_CONTACT_HALFWORDS_H

#include "global.h"

struct PackedContactHalfwords {
    u32 lo : 16; /* confidence=low */
    u32 hi : 16; /* confidence=low */
};

typedef char PackedContactHalfwordsSizeCheck[sizeof(struct PackedContactHalfwords) == 0x4 ? 1 : -1];

#endif
