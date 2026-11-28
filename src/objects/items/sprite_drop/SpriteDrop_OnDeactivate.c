#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/sprite_drop.h"

void Sprite_Remove(void *);

void SpriteDrop_OnDeactivate(struct SpriteDrop *item)
{
    if (item->entered) {
        if (item->sprite.unknown4[0])
            Sprite_Remove(&item->sprite);
        item->active = 0;
        item->entered = 0;
    }
}
