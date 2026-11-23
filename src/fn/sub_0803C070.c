#include "global.h"
#include "scene_pool.h"

int ScenePool_UpdateProximityCooldown(struct ScenePoolActor *p)
{
    struct SpatialEntry *collider = &p->proximityCollider;

    if (p->proximityCooldown > 0) {
        p->proximityCooldown--;
        collider->kind = 0;
        collider->hit = 0;
        return 0;
    }
    if (collider->kind)
        return 1;
    return 0;
}
