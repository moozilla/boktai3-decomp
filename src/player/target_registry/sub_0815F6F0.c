#include "player/target_registry/target_registry.h"

extern struct MotionTargets *volatile gSceneLinkList;

static inline void orm(u32 *a, u32 m)
{
    *a |= m;
}

s32 TargetRegistry_RegisterSceneActor(struct MotionTarget *entry, struct ContactCollider *target,
                                 s32 (*onTarget)(void *, s32, s32))
{
    struct MotionTargets *list = gSceneLinkList;
    struct MotionTarget *cursor;

    if (list == NULL)
        return 0;
    entry->target.collider = target;
    entry->onTarget = onTarget;
    for (cursor = list->first; cursor != NULL; cursor = cursor->next) {
        if (cursor == entry)
            return 1;
    }
    entry->next = gSceneLinkList->first;
    gSceneLinkList->first = entry;
    orm(&entry->flags, MOTION_TARGET_COLLIDER);
    return 1;
}
