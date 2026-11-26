#include "player/target_registry/target_registry.h"

static inline void andm(u32 *a, u32 m)
{
    *a &= m;
}

s32 TargetRegistry_RegisterActor(struct MotionTarget *entry, struct SceneBody *target,
                                 s32 (*onTarget)(void *, s32, s32))
{
    if (gSceneLinkList == NULL)
        return 0;
    entry->target.actor = target;
    entry->onTarget = onTarget;
    if (!TargetRegistry_InsertUnique(entry))
        return 1;
    andm(&entry->flags, ~MOTION_TARGET_COLLIDER);
    return 1;
}
