#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/collectible.h"

void Collectible_OnContact(void *owner, struct SpatialEntry *other)
{
    struct Collectible *item = owner;
    u32 mask = 0x20;
    if (other->flags & mask) {
        item->body.hit = 0;
    } else {
        Collectible_ApplyPickupReward(item, other);
        item->body.kind = other->kind;
    }
}
