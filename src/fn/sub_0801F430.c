#include "scene_object_slot.h"
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
        s->block->w6 &= ~3;
        s->block->w6 |= 2;
        s->block->b4 = (s->block->b4 & 0xf0) | s->b3b;
        s->block->flags |= 1;
        s->callback = SceneObject_UpdateRaised;
    }
    {
        int offset = s->lift;
        s->q0.position.y = s->r0.position.y + offset;
    }
}
