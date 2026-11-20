#ifndef SCENE_OBJECT_CONTROLLER_H
#define SCENE_OBJECT_CONTROLLER_H

#include "scene_animation.h"
#include "scene_object_slot.h"

struct SceneObjPool {
    struct Task task;
    u16 messageId, count;
    u8 primaryCount, secondaryCount;
    u16 primary[4], secondary[4];
    u16 selection;
    s32 capacity;
    s32 separatePalettes;
    u8 *messages;
    struct SceneAnimation animationData[7][5];
    struct SceneAnimation *paletteAnimationData[7];
    struct SceneObjSlot *slots;
    u8 pad430[4];
};

typedef char SceneObjPoolSizeCheck[sizeof(struct SceneObjPool) == 0x434 ? 1 : -1];

#endif
