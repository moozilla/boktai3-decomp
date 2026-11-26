#include "scene/actor/scene_actor_terrain.h"
#include "scene/actor/scene_actor_control_helpers.h"
#include "engine/word_flags.h"
void SceneActorPalette_ClearCompletedTransitions(void);
static inline u8 IsDisposable(struct RoomActor *actor)
{
    if (!SceneActorControl_TestFlags(gEngineFlags, 0x4800)) {
        u32 category = actor->category;
        if (category == 2 || category == 18 || category == 19 || category == 9 || category == 17 ||
            category == 20 || category == 14 || category == 10 || category == 23 || category == 22)
            return 1;
    }
    return 0;
}
void SceneActorTerrain_DisableCollision(struct RoomActor *actor)
{
    struct ContactCollider *collider;
    WordFlags_Set(3, &actor->behaviorFlags);
    collider = &actor->colliderA;
    collider->flags |= 4;
    SceneActorPalette_ClearCompletedTransitions();
    {
        u32 queryMask = 0x800;
        if (WordFlags_Test(&actor->statusFlags, queryMask) && IsDisposable(actor)) {
            u32 removalMask = 0x1000;
            actor->reactionFlags = removalMask | actor->reactionFlags;
        }
    }
}
