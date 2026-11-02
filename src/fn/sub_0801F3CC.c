#include "scene_object_slot.h"
void SceneObject_UpdateLowering(void);
void SceneObject_BeginLowering(struct SceneObjSlot *s)
{
    Sound_PlaySong(0x152);
    s->b39 = 0;
    s->b3a = 0;
    s->block->w6 |= 2;
    s->block->w6 |= 3;
    s->variant = 3;
    s->block->b4 = (s->block->b4 & 0xf0) | (s->b3b + 1);
    s->block->flags &= ~1;
    s->callback = SceneObject_UpdateLowering;
}
