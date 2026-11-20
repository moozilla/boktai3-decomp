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
    slot->pose = pose;
    slot->poseFrame = 0;
}

static inline void SceneObjSlot_EnterIdle(struct SceneObjSlot *slot,
                                          enum SceneObjActivationState state)
{
    slot->variant = state;
    slot->callback =
        state == SCENE_OBJ_ACTIVE ? SceneObject_UpdateActiveIdle : SceneObject_UpdateInactiveIdle;
}

/* Active animations start at the first pose and inactive animations start at the last. */
static inline void SceneObjSlot_ApplyActivation(struct SceneObjSlot *slot,
                                                enum SceneObjActivationState state)
{
    if (state == SCENE_OBJ_ACTIVE || state == SCENE_OBJ_INACTIVE) {
        SceneObjSlot_EnterIdle(slot, state);
    } else {
        slot->variant = state;
        slot->callback =
            state == SCENE_OBJ_ACTIVATING ? SceneObject_AnimateActive : SceneObject_AnimateInactive;
    }
    SceneObjSlot_ResetPose(
        slot, (state == SCENE_OBJ_INACTIVE || state == SCENE_OBJ_DEACTIVATING) ? 2 : 0);
}

#endif
