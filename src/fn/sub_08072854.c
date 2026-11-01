#include "scene_actor_placement.h"
#include "scene_actor_chase_types.h"
#include "scene_actor_init_dispatch.h"

static inline u8 SceneActorTerrain_ExcludesCategory(u8 category)
{
    u32 relative = category - 0x11;
    if (relative <= 3 || category == 0x17)
        return 1;
    return 0;
}
static inline void SceneActorTerrain_InstallControl(struct SceneActorRegistryResetView *actor,
                                                    void *sink, u8 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = sink;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SceneActorTerrain_EnterState(struct SceneActorRegistryResetView *actor, u8 state,
                                                u8 phase, u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
void SceneActorRegistry_ApplyTerrainReactions(void)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct SceneActorRegistryResetView *actor;
    struct ContactPosition center, delta;

    if (!node)
        return;
    actor = (void *)node->actor;
    while (actor) {
        u16 cellFlags;
        if (actor->enabled != 1 || SceneActorTerrain_ExcludesCategory(actor->category)) {
            node = node->next;
            actor = (void *)node->actor;
            continue;
        }
        cellFlags =
            ((struct SceneActorChaseTerrainCell *)SceneActorTerrain_GetCell(actor->terrainCell))
                ->flags;
        if ((cellFlags & 0x40) &&
            actor->body.position.y + (s16)actor->field2EA <= actor->groundHeight2BE) {
            void *sink;
            if (SceneActorPlacement_IsUnavailable((void *)actor)) {
                node = node->next;
                actor = (void *)node->actor;
                continue;
            }
            sink = actor->field238;
            if (!sink) {
                SceneActorTerrain_InstallControl(actor, SceneActorReaction_SinkAndRemove, 0x16);
            } else {
                SceneActorTerrain_InstallControl(actor, sink, 0x16);
            }

            SceneActorTerrain_EnterState(actor, 3, 0, 0);
            MonsterInit_SetStatusFlags(actor, 0x20000);
        } else if ((cellFlags & 0x80) && MonsterInit_TestWorldFlags(0x10)) {
            struct ContactCollider *collider = &actor->colliderA;
            if (actor->colliderA.responseValue) {
                node = node->next;
                actor = (void *)node->actor;
                continue;
            }
            SceneActorTerrain_GetCenter(actor->terrainCell, &center);
            SceneActorTerrain_Subtract(&delta, &center, &actor->body.position);
            actor->field2B6 = Math_Atan2(delta.x, delta.z) >> 5;
            actor->field2E0 = 0x20;
            collider->cost = MonsterInit_GetHealth(actor);
            actor->reactionSoundVariant = MonsterInit_GetHealth(actor);
            collider->responseParameter = 0x28;
            actor->field2D8 = 0x200;
            actor->field2E6 = 1;
            MonsterInit_SetBehaviorFlags(actor, 0x2000);
        }
        node = node->next;
        actor = (void *)node->actor;
    }
}
