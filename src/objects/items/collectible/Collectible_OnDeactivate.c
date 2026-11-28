#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/collectible.h"

void Collectible_OnDeactivate(struct Collectible *item)
{
    if (item->entered) {
        item->active = 0;
        item->entered = 0;
    }
}
