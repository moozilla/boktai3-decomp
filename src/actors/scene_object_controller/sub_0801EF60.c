#include "actors/scene_object_controller/scene_object_controller.h"
void SceneObject_AnimateInactive(struct SceneObjSlot *s)
{
    if ((*(u32 *)&s->dir & 0xffff00) == 0x200)
        Sound_PlaySongChecked(0xcf);
    if (++s->poseFrame > 7) {
        s->pose--;
        s->poseFrame = 0;
        if (s->pose == 0) {
            s->variant = 0;
            s->terrain->cell.flags |= 2;
            s->terrain->cell.flags |= 3;
            s->terrain->cell.shapeAndHeight =
                (s->terrain->cell.shapeAndHeight & 0xf0) | (s->cellHeight + 1);
            s->callback = SceneObject_UpdateInactiveIdle;
        }
    }
}
