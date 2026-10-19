
#ifdef NONMATCHING
// fuzzy: 91.5%
// foundry draft (worker best 91.5%); what differs is mostly register allocation and instruction order
#define RegionActorPool_DestroyActors sub_08106C88
#define gRegionActorPool gUnk_020001B4
#include "global.h"
#include "region_actor.h"
extern struct RegionActorPool *gUnk_020001B4;
void Sprite_Remove(void *);
u32 TerrainOverlay_QueueRemoveActor(u32 *);
void SceneBody_Unregister(void *);
void TerrainModifiers_UnregisterCell(void *);
void Heap_Free(void *);
static inline u8 IsUsed(struct RegionActorPool *pool, s32 index)
{
    u32 *used = &pool->used;
    u32 mask = 1U << index;
    if (*used & mask) return 1;
    return 0;
}
static inline u8 TestFlags(struct RegionActor *actor)
{
    u32 mask = 0x20;
    if (actor->behaviorFlags & mask) return 1;
    return 0;
}
s32 sub_08106C88(struct RegionActorPool *pool)
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
            u32 *flags = &actor->behaviorFlags;
            if (TestFlags(actor)) {
                TerrainModifiers_UnregisterCell(actor->resource);
                { u32 mask = ~0x20; *flags &= mask; }
            }
            }
            Heap_Free(actor);
        }
    }
    gUnk_020001B4 = 0;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08106C88);
#endif
