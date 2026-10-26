#include "bouncing_object.h"
void ExplosionEffect_Create(struct ContactPosition *, s32, u32, u32, u32);
void SceneStimulus_AddTagged(s32, s32, s32, s32, s32, u32);
void BouncingObject_TickRemoval(struct BouncingPool *unused, struct BouncingObject *object)
{
    s8 *index;
    if (object->enter) {
        ExplosionEffect_Create(&object->sprite.position, 32, object->spawnParameterA,
                               object->spawnParameterB, 0);
        SceneStimulus_AddTagged(object->sprite.position.x, object->sprite.position.z, 1024, 0, 1,
                                (u16)object->colliderConfig);
        object->enter = 0;
    }
    {
        struct BouncingPool *pool = gBouncingPool;
        s32 id;
        u32 *used;
        index = &object->index;
        id = *index;
        used = &pool->used;
        *used &= ~(1U << id);
    }
    object->state = 0;
    Sprite_Remove(object);
    Shadow_Destroy(object->registration);
    Collider_Remove(&object->removalCollider);
    *(u8 *)index = 255;
    object->flags = 0;
    object->timer = 0;
}
