#include "scene_object_slot.h"
void SceneObject_UpdateLowered(void);
struct SceneObjPool {
    u8 pad0[0x1a];
    u16 count;
    u8 pad1c[0x410];
    struct SceneObjSlot *slots;
};
static inline u8 SceneObjects_GetVariantByte(int value)
{
    return value;
}
int SceneObjects_ScriptLower(void)
{
    struct SceneObjPool *p = ResourceRegistry_GetResource(0x85e6);
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
    if (found < p->count) {
        struct SceneObjSlot **slots = &p->slots;
        int one;
        u32 mask;
        int zero = 0;
        do {
            struct SceneObjSlot *s = *slots + i;
            if (s->id == id) {
                s->timer = zero;
                one = 1;
                s->variant = SceneObjects_GetVariantByte(one);
                s->callback = fn;
                s->b39 = *(u8 *)&s->loweredPose;
                s->b3a = 0;
                mask = (u16)~2;
                s->block->w6 &= mask;
                s->block->w6 |= 3;
                s->block->b4 = (s->block->b4 & 0xf0) | (s->b3b + 1);
                s->lift = zero;
                s->spritePos.position.y = s->q0.position.y;
                found = 1;
            }
            i++;
        } while (i < p->count);
    }
    return found;
}
