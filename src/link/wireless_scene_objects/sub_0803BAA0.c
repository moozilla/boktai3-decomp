#include "engine/shared/global.h"
#include "engine/shared/scene_pool.h"

struct ScenePoolActor *ScenePool_FindFreeActorSlot(struct ScenePoolManager *p)
{
    struct ScenePoolActor *slot = p->actors;
    u32 i;

    for (i = 0; i < p->configuration; i++, slot++) {
        if (slot->entryIndex == -1)
            return slot;
    }
    return 0;
}
void Actor_UpdateShadowSpriteParts(void *, void *);
void ScenePool_UpdateActorDrawablePosition(struct ScenePoolActor *p)
{
    Actor_UpdateShadowSpriteParts(p, p->model);
}
void ScenePool_ResetProximityContact(struct ScenePoolActor *p)
{
    struct SpatialEntry *collider = &p->proximityCollider;
    collider->hit = 0;
    collider->kind = 0;
}
