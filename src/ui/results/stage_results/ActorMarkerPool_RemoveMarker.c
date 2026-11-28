#include "global.h"
#include "ui/results/actor_marker_pool.h"

void Sprite_Remove(void *);

void ActorMarkerPool_RemoveMarker(u32 i)
{
    struct MarkerPool *pool = (struct MarkerPool *)gPanelSpritePool;
    if (pool != 0 && i < 8 && pool->slots[i].sprite.unknown4[0] != 0) {
        Sprite_Remove(&pool->slots[i].sprite);
        pool->slots[i].active = 0;
    }
}
