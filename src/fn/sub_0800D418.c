#include "falling_objects.h"
void sub_08014634(u32, u32, void *, void *, void *, u32, u32);
void sub_082260A4(u32);
void FallingObject_UpdateBounceAway(struct FallingObject *object)
{
    struct FallingPackedVec low, high;
    if (object->entered) {
        object->entered = 0;
        if (!(gUnk_030053F4 & 2048)) {
            low.x = -15;
            low.y = 8;
            low.z = -15;
            high.x = 30;
            high.y = 22;
            high.z = 30;
            sub_08014634(8, 3, &object->position, &low, &high, 60, 60);
        }
        object->parameter = 25;
    }
    if (object->timer < 15) {
        object->position.y -= (object->timer >> 2) - 16;
        sub_082260A4(40);
    } else {
        s32 amount = object->timer - 15;
        if (amount < 16)
            object->position.y -= amount;
        else
            object->position.y -= 16;
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
    if (object->timer++ > object->parameter) {
        sub_0800CC34(object);
        object->remove = 1;
    }
}
