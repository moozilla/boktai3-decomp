#include "global.h"
#include "ui/results/actor_marker_pool.h"

u8 *ActorMarkerPool_EnsureSingleton(void);

s32 SceneSlot_RegisterObject(const struct FixedPosition *anchor)
{
    struct MarkerPool *pool;
    s32 i;
    if (gPanelSpritePool) {
        pool = (struct MarkerPool *)gPanelSpritePool;
    } else {
        pool = (struct MarkerPool *)ActorMarkerPool_EnsureSingleton();
        if (!pool)
            return -1;
    }
    for (i = 0; i < 8; i++) {
        if (!pool->slots[i].active) {
            pool->slots[i].anchor = anchor;
            pool->slots[i].sprite.flags &= ~1;
            pool->slots[i].active = 1;
            return i;
        }
    }
    return -1;
}
