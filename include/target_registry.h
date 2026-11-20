#ifndef GUARD_TARGET_REGISTRY_H
#define GUARD_TARGET_REGISTRY_H

#include "task.h"

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

#endif
