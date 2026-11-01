#include "bouncing_object.h"
void ExplosionEffect_Create(struct ContactPosition *, s32, u32, u32, u32);
void SceneStimulus_AddTagged(s32, s32, s32, s32, s32, u32);
void BouncingObject_TickRemoval(struct BouncingPool *unused, struct BouncingObject *object)
{
    if (object->enter) {
        ExplosionEffect_Create(&object->sprite.position, 32, object->spawnParameterA,
                               object->spawnParameterB, 0);
        SceneStimulus_AddTagged(object->sprite.position.x, object->sprite.position.z, 1024, 0, 1,
                                (u16)object->colliderConfig);
        object->enter = 0;
    }
    BouncingObject_ReleaseSlot(object);
}
