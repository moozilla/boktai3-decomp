#include "scene_object_slot.h"
void SceneObject_UpdateActiveIdle(void);
void SceneObject_AnimateActive(struct SceneObjSlot *s)
{
    if ((*(u32 *)&s->dir & 0xffff00) == 0) {
        Sound_PlaySongChecked(0xce);
        s->block->w6 &= ~3;

        s->block->w6 |= 2;
        s->block->b4 = (s->block->b4 & 0xf0) | s->b3b;
    }
    if (++s->b3a > 7) {
        s->b39++;
        s->b3a = 0;
        if (s->b39 > 1) {
            {
                int one = 1;
                s->variant = one;
            }
            s->callback = SceneObject_UpdateActiveIdle;
        }
    }
}
