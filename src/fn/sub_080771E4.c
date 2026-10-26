#include "scene_actor_terrain.h"
void SceneActorPalette_ClearCompletedTransitions(void);
static inline u32 ReadMasked(const u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u8 TestFlags(u32 flags, u32 mask)
{
    if (flags & mask)
        return 1;
    return 0;
}
static inline u8 IsDisposable(struct SceneActorTerrainCollisionView *actor)
{
    if (!TestFlags(gEngineFlags, 0x4800)) {
        u32 category = actor->category;
        if (category == 2 || category == 18 || category == 19 || category == 9 || category == 17 ||
            category == 20 || category == 14 || category == 10 || category == 23 || category == 22)
            return 1;
    }
    return 0;
}
void SceneActorTerrain_DisableCollision(struct SceneActorTerrainCollisionView *actor)
{
    struct ContactCollider *collider;
    u32 mask = 3;
    actor->flags2CC |= mask;
    collider = &actor->collider;
    collider->flags |= 4;
    SceneActorPalette_ClearCompletedTransitions();
    {
        u32 queryMask = 0x800;
        if (ReadMasked(&actor->flags2D0, queryMask) && IsDisposable(actor)) {
            u32 removalMask = 0x1000;
            actor->flags2D6 = removalMask | actor->flags2D6;
        }
    }
}
