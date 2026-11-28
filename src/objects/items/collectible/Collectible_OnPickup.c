#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/collectible.h"

void Collectible_OnPickup(void *owner, struct SpatialEntry *other)
{
    struct Collectible *item = owner;
    struct SpatialEntry *body = &item->body;
    u32 mask = 1;
    body->flags |= mask;
    Collectible_ApplyPickupReward(item, other);
    item->update = Collectible_StartDeath;
    item->entered = mask;
}
