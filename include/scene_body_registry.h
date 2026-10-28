#ifndef GUARD_SCENE_BODY_REGISTRY_H
#define GUARD_SCENE_BODY_REGISTRY_H

#include "global.h"

struct SceneBodyRegistry {
    u8 unk00[0x18];
    void *first, *last;
};

extern struct SceneBodyRegistry *gSceneBodyRegistry;

#endif
