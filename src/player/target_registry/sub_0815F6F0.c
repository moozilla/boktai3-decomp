#include "player/target_registry/target_registry.h"

static inline void orm(u32 *a, u32 m)
{
    *a |= m;
}

s32 TargetRegistry_RegisterSceneActor(struct MotionTarget *entry, struct ContactCollider *target,
                                      s32 (*onTarget)(void *, s32, s32))
{
    if (gSceneLinkList == NULL)
        return 0;
    entry->target.collider = target;
    entry->onTarget = onTarget;
    if (!TargetRegistry_InsertUnique(entry))
        return 1;
    orm(&entry->flags, MOTION_TARGET_COLLIDER);
    return 1;
}
