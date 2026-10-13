#include "falling_objects.h"
void sub_08015F60(u32, void *, u32, u32, u32, u32, u32, u32, u32, u32);
void FallingObject_UpdateWaiting(struct FallingObject *object)
{
    if (object->entered) {
        object->entered = 0;
        Anim_SetSequence(&object->anim, gUnk_0200046C->animation, 2, 0, 0);
    }
    if (object->health <= 0 || object->timer++ > object->parameter) {
        FallingObject_SetState(object, 3);
        return;
    }
    if (object->collider)
        Collider_SetPosition(object->collider, &object->position, 0);
    if (object->attack) {
        Collider_SetPosition(object->attack, &object->position, 0);
        Collider_Refresh(object->attack);
    }
}
