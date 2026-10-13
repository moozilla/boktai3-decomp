#include "falling_objects.h"
void sub_08015F60(u32, void *, u32, u32, u32, u32, u32, u32, u32, u32);
void FallingObject_SpawnFinishParticles(void *);
void FallingObject_UpdateFinish(struct FallingObject *object)
{
    if (object->entered) {
        object->entered = 0;
        Anim_SetSequence(&object->anim, gUnk_0200046C->animation, 4, 0, 0);
        if (object->collider) {
            u32 flags = ((u16 *)object->collider)[3];
            u32 result;
            result = 4;
            result |= flags;
            ((u16 *)object->collider)[3] = result;
        }
        sub_0800CC34(object);
    }
    if (object->animationDone) {
        if (!object->remove)
            FallingObject_SpawnFinishParticles(object);
        object->remove = 1;
    }
}
