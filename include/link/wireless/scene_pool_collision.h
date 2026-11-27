#ifndef GUARD_SCENE_POOL_COLLISION_H
#define GUARD_SCENE_POOL_COLLISION_H

#include "link/wireless/scene_pool.h"

static inline struct SpatialEntry *ScenePool_MotionCollider(struct ScenePoolActor *actor)
{
    return &actor->motionCollider;
}

#endif
