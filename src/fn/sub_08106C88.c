#include "global.h"
#include "region_actor.h"
#include "ram.h"

void Sprite_Remove(void *);
u32 TerrainOverlay_QueueRemoveActor(u32 *);
void SceneBody_Unregister(void *);
void TerrainModifiers_UnregisterCell(void *);
void Heap_Free(void *);
static inline u8 IsUsed(struct RegionActorPool *pool, s32 index)
{
    u32 *used = &pool->used;
    u32 mask = 1U << index;
    if (*used & mask)
        return 1;
    return 0;
}
static inline u8 TestFlagsValue(u32 v)
{
    if (v)
        return 1;
    return 0;
}
s32 RegionActorPool_DestroyActors(struct RegionActorPool *pool)
{
    s32 i;
    for (i = 0; i < 32; i++) {
        if (IsUsed(pool, i)) {
            struct RegionActor *actor = pool->actors[i];
            Sprite_Remove(actor);
            if (actor->overlayActive) {
                TerrainOverlay_QueueRemoveActor(actor->overlay);
                SceneBody_Unregister(actor->overlay);
            }
            {
                u32 m20 = 0x20;
                u32 *flags;
                u32 *t;
                u32 v;
                t = &actor->behaviorFlags;
                v = *t;
                v &= m20;
                flags = t;
                if (TestFlagsValue(v)) {
                    TerrainModifiers_UnregisterCell(actor->resource);
                    {
                        u32 mask = ~0x20;
                        *flags &= mask;
                    }
                }
            }
            Heap_Free(actor);
        }
    }
    gRegionActorPool = 0;
    return 0;
}
