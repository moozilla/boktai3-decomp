#define gRegionActorUpdateCallbacks gUnk_086064FC
#include "global.h"
#include "game_state.h"
#include "region_actor.h"
extern void *gUnk_086064FC[];
s32 Callback_InvokeActor(void *, void *);
static inline u8 IsUsed(u32 *used, u32 mask)
{
    if (*used & mask)
        return 1;
    return 0;
}
static inline u8 TestFlags(struct RegionActor *actor)
{
    u32 mask = 0x80;
    if (actor->behaviorFlags & mask)
        return 1;
    return 0;
}
static inline u8 IsCurrentRegion(struct RegionActor *actor)
{
    s32 region = actor->region;
    if (gGameState->region == region)
        return 1;
    return 0;
}
s32 RegionActorPool_UpdateActors(struct RegionActorPool *pool)
{
    struct RegionActor **actors = pool->actors;
    s32 i;
    void **callbacks;
    u32 *used;
    u8 *cursor;
    if (pool->used) {
        i = 0;
        used = &pool->used;
        cursor = &pool->cursor;
        callbacks = gUnk_086064FC;
        do {
            if (IsUsed(used, 1U << i)) {
                if (TestFlags(actors[i]) && !IsCurrentRegion(actors[i]))
                    actors[i]->flags |= 1;
                else
                    Callback_InvokeActor(actors[i], callbacks[actors[i]->kind]);
            }
            i++;
        } while (i < 32);
        *cursor = 0xFF;
    }
    return 0;
}
