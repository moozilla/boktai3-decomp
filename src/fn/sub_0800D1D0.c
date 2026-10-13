#include "falling_objects.h"
void sub_08015F60(u32, void *, u32, u32, u32, u32, u32, u32, u32, u32);
void FallingObject_UpdateLanding(struct FallingObject *object)
{
    if (object->entered) {
        object->entered = 0;
        Anim_SetSequence(&object->anim, gUnk_0200046C->animation, 2, 0, 0);
        sub_0800CC34(object);
        sub_08015F60((FallingObject_Random() & 3) + 6, &object->position, 9, 20, 8, 8, 0, 256, 24, 16);
    }
    if (object->type > 1) {
        object->frameTimer++;
        if (object->frameTimer == 10) {
            object->frame++;
            if (object->frame > 2)
                object->frame = 0;
            object->frameTimer = 0;
        }
        FallingObject_SetFrame(&object->sprite, object->frame);
    }
    if (object->health <= 0) {
        FallingObject_SetState(object, 3);
        return;
    }
    if (object->timer++ > 30) {
        FallingObject_SetState(object, 2);
    } else {
        s32 amount = (30 - object->timer) >> 1;
        if (!(object->timer & 1)) {
            object->sprite.position.x += amount;
            object->sprite.position.z -= amount;
        } else {
            object->sprite.position.x -= amount;
            object->sprite.position.z += amount;
        }
    }
    if (object->collider)
        Collider_SetPosition(object->collider, &object->position, 0);
    if (object->attack) {
        Collider_SetPosition(object->attack, &object->position, 0);
        Collider_Refresh(object->attack);
    }
}
