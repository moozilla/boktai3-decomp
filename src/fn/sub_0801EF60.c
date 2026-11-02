#include "scene_object_slot.h"
void SceneObject_UpdateInactiveIdle(void);
void SceneObject_AnimateInactive(struct SceneObjSlot *s)
{
    if ((*(u32 *)&s->dir & 0xffff00) == 0x200)
        Sound_PlaySongChecked(0xcf);
    if (++s->b3a > 7) {
        s->b39--;
        s->b3a = 0;
        if (s->b39 == 0) {
            s->variant = 0;
            s->block->w6 |= 2;
            s->block->w6 |= 3;
            s->block->b4 = (s->block->b4 & 0xf0) | (s->b3b + 1);
            s->callback = SceneObject_UpdateInactiveIdle;
        }
    }
}
