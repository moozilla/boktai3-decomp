#include "route_sprite_actor.h"
struct RouteSpriteActor *RouteSpritePool_TryAllocateSlot(struct RouteSpritePool *pool)
{
    struct RouteSpriteActor *slot = RouteSpritePool_AllocateSlot(pool);
    if (!slot)
        return NULL;
    return (void *)&slot->sprite;
}
