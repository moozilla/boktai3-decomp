#include "global.h"

#include "bouncing_object.h"
#include "engine.h"
struct BouncingObject *BouncingObject_Spawn(struct ContactPosition *source,
                                            struct ContactPosition *delta,
                                            struct ContactPosition *target, u32 shot, u32 category,
                                            u32 strength, u32 id)
{
    struct BouncingPool *pool;
    struct BouncingObject *object;
    struct BouncingObject *sprite;
    u32 zero;
    u32 one;
    pool = gBouncingPool;
    if (!pool) {
        pool = BouncingObject_EnsurePool();
        if (!pool)
            return NULL;
    }
    if (pool->stopping)
        return NULL;
    {
        u32 mask = 0x400;
        if ((gEngineFlags | gScenePauseFlags) & mask)
            return NULL;
    }
    zero = gUnk_02000560 & 4;
    if (zero)
        return NULL;
    object = BouncingObject_Allocate(pool);
    if (!object)
        return NULL;
    {
        u8 *state = &object->state;
        one = 1;
        *state = one;
    }
    object->flags = zero;
    object->origin = *target;
    object->enter = one;
    object->velocity = *delta;
    object->spawnParameterA = shot;
    object->spawnParameterB = category;
    object->initialTimer = strength;
    object->timer = strength;
    object->collisionDelay = 30;
    object->colliderConfig = id;
    object->animation = one;
    sprite = object;
    object->sprite.position = *source;
    Sprite_Init(object, pool->graphics, 0);
    Anim_SetSequence(&object->sequence, pool->sequenceResource, 1, zero, 0);
    BouncingObject_StepSequence(object, &sprite->sprite, &object->sequence, one);
    sprite->sprite.flags |= 1;
    sprite->sprite.priority = 1;
    Shadow_Initialize(object->registration, &sprite->sprite.position, 0);
    BouncingObject_InitializeColliders(object, 0, 0);
    return object;
}
