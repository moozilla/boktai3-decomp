#include "objects/scene_objects/scene_object_slot.h"
void SceneObject_UpdateRaised(void);
void SceneObject_UpdateRaising(struct SceneObjSlot *s)
{
    s->lift += 15;
    if ((s16)s->lift > (s16)s->maxLift) {
        s->lift = s->maxLift;
        Sound_StopSong(0x152);
        Sound_PlaySong(0x154);
        s->timer = 30;
        s->variant = 0;
        s->terrain->cell.flags &= ~3;
        s->terrain->cell.flags |= 2;
        s->terrain->cell.shapeAndHeight = (s->terrain->cell.shapeAndHeight & 0xf0) | s->cellHeight;
        s->terrain->flags |= 1;
        s->callback = SceneObject_UpdateRaised;
    }
    {
        int offset = s->lift;
        s->q0.position.y = s->r0.position.y + offset;
    }
}
