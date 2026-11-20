#include "scene_object_controller.h"

void SceneObject_AnimateActive(struct SceneObjSlot *s)
{
    if ((*(u32 *)&s->dir & 0xffff00) == 0) {
        Sound_PlaySongChecked(0xce);
        s->terrain->cell.flags &= ~3;

        s->terrain->cell.flags |= 2;
        s->terrain->cell.shapeAndHeight = (s->terrain->cell.shapeAndHeight & 0xf0) | s->cellHeight;
    }
    if (++s->poseFrame > 7) {
        s->pose++;
        s->poseFrame = 0;
        if (s->pose > 1) {
            SceneObjSlot_EnterIdle(s, SCENE_OBJ_ACTIVE);
        }
    }
}
