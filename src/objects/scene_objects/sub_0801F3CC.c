#include "objects/scene_objects/scene_object_slot.h"
void SceneObject_UpdateLowering(void);
void SceneObject_BeginLowering(struct SceneObjSlot *s)
{
    Sound_PlaySong(0x152);
    s->pose = 0;
    s->poseFrame = 0;
    s->terrain->cell.flags |= 2;
    s->terrain->cell.flags |= 3;
    s->variant = SceneObjSlot_LiftMotionVariant(SCENE_OBJ_LIFT_LOWERED);
    s->terrain->cell.shapeAndHeight = (s->terrain->cell.shapeAndHeight & 0xf0) | (s->cellHeight + 1);
    s->terrain->flags &= ~1;
    s->callback = SceneObject_UpdateLowering;
}
