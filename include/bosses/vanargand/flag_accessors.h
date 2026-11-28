#ifndef GUARD_VANARGAND_FLAG_ACCESSORS_H
#define GUARD_VANARGAND_FLAG_ACCESSORS_H

#include "scene/collision/spatial_entry.h"
#include "player/target_registry/target_registry.h"
#include "player/player.h"
#include "scene/collision/contact_collider.h"

static inline void SpatialEntry_SetFlags(struct SpatialEntry *p, u32 mask)
{
    p->flags |= mask;
}

static inline void MotionTarget_KeepFlags(struct MotionTarget *p, s32 mask)
{
    p->flags &= mask;
}

static inline u32 Player_TestStatusFlags(struct Player *p, u32 mask)
{
    return p->statusFlags & mask;
}

static inline u32 ContactCollider_TestStatRewardFlags(struct ContactCollider *p, u32 mask)
{
    return p->statRewardFlags & mask;
}

#endif
