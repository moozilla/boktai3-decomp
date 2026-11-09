#include "scene_object_slot.h"
void SceneObject_UpdateLowered(void);
int SceneObjects_ScriptLower(void)
{
    u8 *p = ResourceRegistry_GetResource(0x85e6);
    int found;
    int i;
    int id;
    void (*fn)(void) = SceneObject_UpdateLowered;
    if (!p)
        return -1;
    found = 0;
    if (!Script_SeekToKeyword(0x6e))
        return -1;
    id = Script_GetValue();
    i = 0;
    if (found < *(u16 *)(p + 0x1a)) {
        struct SceneObjSlot **slots = (struct SceneObjSlot **)(p + 0x42c);
        int zero = 0;
        int one = 1;
        do {
            struct SceneObjSlot *s = *slots + i;
            if (s->id == id) {
                s->timer = zero;
                s->variant = one;
                s->callback = fn;
                s->b39 = *(u8 *)&s->loweredPose;
                s->b3a = 0;
                s->block->w6 &= ~2;
                s->block->w6 |= 3;
                s->block->b4 = (s->block->b4 & 0xf0) | (s->b3b + 1);
                s->lift = zero;
                s->spritePos.position.y = s->q0.position.y;
                found = 1;
            }
            i++;
        } while (i < *(u16 *)(p + 0x1a));
    }
    return found;
}
