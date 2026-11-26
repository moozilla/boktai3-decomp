#include "objects/scene_objects/scene_object_controller.h"

void SceneObject_UpdateRaising(struct SceneObjSlot *);

void SceneObject_BeginRaising(struct SceneObjSlot *slot)
{
    Sound_PlaySong(0x152);
    SceneObjSlot_ResetPose(slot, 0);
    slot->variant = SceneObjSlot_LiftMotionVariant(SCENE_OBJ_LIFT_RAISED);
    slot->callback = SceneObject_UpdateRaising;
}
