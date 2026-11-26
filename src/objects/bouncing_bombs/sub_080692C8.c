#include "global.h"

#include "objects/bouncing_bombs/bouncing_object.h"
#include "engine/engine.h"
#include "engine/engine_pause.h"
struct BouncingObject *BouncingBomb_Spawn(struct ContactPosition *source,
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
        pool = BouncingBomb_EnsurePool();
        if (!pool)
            return NULL;
    }
    if (pool->stopping)
        return NULL;
    {
        if (Engine_PauseBits(0x400))
            return NULL;
    }
    zero = gUnk_02000560 & 4;
    if (zero)
        return NULL;
    object = BouncingBomb_Allocate(pool);
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
    BouncingBomb_StepSequence(object, &sprite->sprite, &object->sequence, one);
    sprite->sprite.flags |= 1;
    sprite->sprite.priority = 1;
    Shadow_Initialize(object->registration, &sprite->sprite.position, 0);
    BouncingBomb_InitializeColliders(object, 0, 0);
    return object;
}
