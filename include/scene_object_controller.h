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

void SceneObject_AnimateActive(struct SceneObjSlot *);
void SceneObject_AnimateInactive(struct SceneObjSlot *);
void SceneObject_UpdateActiveIdle(struct SceneObjSlot *);
void SceneObject_UpdateInactiveIdle(struct SceneObjSlot *);

static inline void SceneObjSlot_ResetPose(struct SceneObjSlot *slot, u8 pose)
{
    slot->b39 = pose;
    slot->b3a = 0;
}

/* Active animations start at the first pose and inactive animations start at the last. */
static inline void SceneObjSlot_ApplyActivation(struct SceneObjSlot *slot,
                                                enum SceneObjActivationState state)
{
    slot->variant = state;
    switch (state) {
        case SCENE_OBJ_INACTIVE:
            slot->callback = SceneObject_UpdateInactiveIdle;
            break;
        case SCENE_OBJ_ACTIVE:
            slot->callback = SceneObject_UpdateActiveIdle;
            break;
        case SCENE_OBJ_ACTIVATING:
            slot->callback = SceneObject_AnimateActive;
            break;
        case SCENE_OBJ_DEACTIVATING:
            slot->callback = SceneObject_AnimateInactive;
            break;
    }
    SceneObjSlot_ResetPose(slot,
                           (state == SCENE_OBJ_INACTIVE || state == SCENE_OBJ_DEACTIVATING) ? 2 : 0);
}

#endif
