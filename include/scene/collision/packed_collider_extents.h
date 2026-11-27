#ifndef GUARD_INCLUDE_SCENE_COLLISION_PACKED_COLLIDER_EXTENTS_H
#define GUARD_INCLUDE_SCENE_COLLISION_PACKED_COLLIDER_EXTENTS_H

#include "global.h"
#include "scene/collision/packed_contact_halfwords.h"

struct PackedColliderExtents {
    struct PackedContactHalfwords xy;
    struct PackedContactHalfwords zAndPad;
};

typedef char PackedColliderExtentsSizeCheck[sizeof(struct PackedColliderExtents) == 0x8 ? 1 : -1];

#endif
