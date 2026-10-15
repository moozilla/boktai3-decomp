#include "bouncing_object.h"
void sub_082333A8(struct ContactPosition *, s32, u32, u32, u32);
void sub_0822B7B4(s32, s32, s32, s32, s32, u32);
void BouncingObject_TickRemoval(struct BouncingPool *unused, struct BouncingObject *object)
{
    s8 *index;
    if (object->enter) {
        sub_082333A8(&object->sprite.position, 32, object->spawnParameterA, object->spawnParameterB,
                     0);
        sub_0822B7B4(object->sprite.position.x, object->sprite.position.z, 1024, 0, 1,
                     (u16)object->colliderConfig);
        object->enter = 0;
    }
    {
        struct BouncingPool *pool = gUnk_0200014C;
        s32 id;
        u32 *used;
        index = &object->index;
        id = *index;
        used = &pool->used;
        *used &= ~(1U << id);
    }
    object->state = 0;
    Sprite_Remove(object);
    sub_08013684(object->registration);
    Collider_Remove(&object->removalCollider);
    *(u8 *)index = 255;
    object->flags = 0;
    object->timer = 0;
}
