#include "route_sprite_actor.h"
#define RouteSpritePool_TryAllocateSlot sub_0806DFB8
struct RouteSpriteActor *RouteSpritePool_TryAllocateSlot(struct RouteSpritePool *pool)
{
    struct RouteSpriteActor *slot = sub_0806DF60(pool);
    if (!slot)
        return NULL;
    return (void *)&slot->sprite;
}
