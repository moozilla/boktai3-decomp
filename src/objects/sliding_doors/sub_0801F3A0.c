#include "objects/sliding_doors/scene_object_controller.h"

void SlidingDoor_UpdateRaising(struct SceneObjSlot *);

void SlidingDoor_BeginRaising(struct SceneObjSlot *slot)
{
    Sound_PlaySong(0x152);
    SceneObjSlot_ResetPose(slot, 0);
    slot->variant = SceneObjSlot_LiftMotionVariant(SCENE_OBJ_LIFT_RAISED);
    slot->callback = SlidingDoor_UpdateRaising;
}
