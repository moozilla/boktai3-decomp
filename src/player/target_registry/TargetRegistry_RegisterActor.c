#include "player/target_registry/target_registry.h"
#include "scene/motion/scene_actor_chase_shared.h"

extern struct MotionTargets *volatile gSceneLinkList;

s32 TargetRegistry_RegisterActor(struct MotionTarget *entry, struct SceneBody *target,
                                 s32 (*onTarget)(void *, s32, s32))
{
    struct MotionTargets *list = gSceneLinkList;
    struct MotionTarget *cursor;

    if (list == NULL)
        return 0;
    entry->target.actor = target;
    entry->onTarget = onTarget;
    for (cursor = list->first; cursor != NULL; cursor = cursor->next) {
        if (cursor == entry)
            return 1;
    }
    entry->next = gSceneLinkList->first;
    gSceneLinkList->first = entry;
    SceneActorChase_AndWordFlags(&entry->flags, ~MOTION_TARGET_COLLIDER);
    return 1;
}
