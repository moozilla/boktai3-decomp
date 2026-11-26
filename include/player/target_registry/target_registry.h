#ifndef GUARD_TARGET_REGISTRY_H
#define GUARD_TARGET_REGISTRY_H

#include "engine/shared/ram.h"
#include "engine/shared/task.h"

struct ContactCollider;
struct SceneBody;

union MotionTargetObject {
    struct SceneBody *actor;
    struct ContactCollider *collider;
};

enum { MOTION_TARGET_COLLIDER = 0x10 };

struct MotionTarget {
    union MotionTargetObject target;
    u32 flags;
    s32 (*onTarget)(void *, s32, s32);
    struct MotionTarget *next;
};

struct MotionTargets {
    struct Task task;
    struct MotionTarget *first;
    u32 unknown1C;
};

/* Prepend an absent entry to the initialized registry. */
static inline bool32 TargetRegistry_InsertUnique(struct MotionTarget *entry)
{
    struct MotionTarget *cursor = gSceneLinkList->first;

    if (cursor == NULL)
        goto linkShortList;
    if (cursor == entry)
        return FALSE;
    cursor = cursor->next;
    if (cursor == NULL)
        goto linkShortList;
    for (; cursor != NULL; cursor = cursor->next) {
        if (cursor == entry)
            return FALSE;
    }
    entry->next = gSceneLinkList->first;
    goto publishHead;
linkShortList:
    entry->next = gSceneLinkList->first;
publishHead:
    gSceneLinkList->first = entry;
    return TRUE;
}

#endif
