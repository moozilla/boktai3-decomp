#ifndef GUARD_SCENE_BODY_REGISTRY_H
#define GUARD_SCENE_BODY_REGISTRY_H

#include "global.h"
#include "task.h"

struct SceneBodyRegistry {
    struct Task task;
    void *first, *last;
};

/* 0x030025F8 */
extern struct SceneBodyRegistry *gSceneBodyRegistry;

#endif
