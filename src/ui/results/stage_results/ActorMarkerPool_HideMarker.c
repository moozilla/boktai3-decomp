#include "global.h"
#include "ui/results/actor_marker_pool.h"

void ActorMarkerPool_HideMarker(s32 i)
{
    if (i >= 0) {
        struct MarkerPool *pool = (struct MarkerPool *)gPanelSpritePool;
        if (pool != 0 && pool->slots[i].active != 0)
            pool->slots[i].sprite.flags |= 1;
    }
}
