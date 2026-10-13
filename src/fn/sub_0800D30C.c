#include "falling_objects.h"
void sub_08014634(u32, u32, void *, void *, void *, u32, u32);
void sub_082260A4(u32);
void FallingObject_UpdateShatter(struct FallingObject *object)
{
    struct FallingPackedVec low, high;
    if (object->entered) {
        object->entered = 0;
        sub_0800CC34(object);
        if (!(gUnk_030053F4 & 2048)) {
            low.x = -15;
            low.y = 8;
            low.z = -15;
            high.x = 30;
            high.y = 22;
            high.z = 30;
            sub_08014634(8, 3, &object->position, &low, &high, 60, 60);
        }
        object->parameter = 30;
    }
    if (object->timer < 12) {
        object->position.y += 8;
        sub_082260A4(40);
    } else {
        object->position.y -= 4;
        if (object->position.y < object->ground)
            object->position.y = object->ground;
    }
    if (object->collider)
        Collider_SetPosition(object->collider, &object->position, 0);
    if (object->attack) {
        Collider_SetPosition(object->attack, &object->position, 0);
        Collider_Refresh(object->attack);
    }
    if ((object->timer & 7) < 2)
        object->sprite.flags |= 1;
    else
        object->sprite.flags &= ~1;
    if (object->timer++ > object->parameter)
        object->remove = 1;
}
