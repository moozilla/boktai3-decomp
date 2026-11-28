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
    struct BouncingObject *self;

    pool = gBouncingPool;
    if (!pool) {
        pool = BouncingBomb_EnsurePool();
        if (!pool)
            return NULL;
    }
    if (pool->stopping)
        return NULL;
    if (Engine_PauseBits(0x400))
        return NULL;
    if (gUnk_02000560 & 4)
        return NULL;
    object = BouncingBomb_Allocate(pool);
    if (!object)
        return NULL;
    object->state = 1;
    object->flags = 0;
    object->origin = *target;
    object->enter = 1;
    object->velocity = *delta;
    object->spawnParameterA = shot;
    object->spawnParameterB = category;
    object->initialTimer = strength;
    object->timer = strength;
    object->collisionDelay = 30;
    object->colliderConfig = id;
    object->animation = 1;
    self = object;
    object->sprite.position = *source;
    Sprite_Init(object, pool->graphics, 0);
    Anim_SetSequence(&object->sequence, pool->sequenceResource, 1, 0, 0);
    BouncingBomb_StepSequence(object, &self->sprite, &object->sequence, 1);
    self->sprite.flags |= 1;
    self->sprite.priority = 1;
    Shadow_Initialize(object->registration, &self->sprite.position, 0);
    BouncingBomb_InitializeColliders(object, 0, 0);
    return object;
}
