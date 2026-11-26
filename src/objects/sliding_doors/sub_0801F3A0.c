#include "objects/sliding_doors/scene_object_controller.h"

void SlidingDoors_UpdateRaising(struct SceneObjSlot *);

void SlidingDoors_BeginRaising(struct SceneObjSlot *slot)
{
    Sound_PlaySong(0x152);
    SceneObjSlot_ResetPose(slot, 0);
    slot->variant = SceneObjSlot_LiftMotionVariant(SCENE_OBJ_LIFT_RAISED);
    slot->callback = SlidingDoors_UpdateRaising;
}
