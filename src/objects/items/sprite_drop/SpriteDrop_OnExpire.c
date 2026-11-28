#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/sprite_drop.h"

void SpriteDrop_OnExpire(void *owner, struct SpatialEntry *other)
{
    struct SpriteDrop *item = owner;
    item->update = SpriteDrop_OnDeactivate;
    item->entered = 1;
}
