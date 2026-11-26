#include "scene/actor/scene_actor_placement.h"
#include "scene/motion/scene_actor_chase_types.h"
#include "scene/actor/scene_actor_init_dispatch.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_registry.h"
#include "engine/engine.h"
#include "scene/actor/scene_actor_components.h"
#include "scene/actor/scene_actor_control_helpers.h"

void SceneActorRegistry_ApplyTerrainReactions(void)
{
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct ChaseActor *actor;
    struct ContactPosition center, delta;

    if (!node)
        return;
    actor = (void *)node->actor;
    while (actor) {
        u16 cellFlags;
        if (actor->base.enabled != 1 || SceneActorTerrain_ExcludesCategory(actor->base.category)) {
            node = node->next;
            actor = (void *)node->actor;
            continue;
        }
        cellFlags = ((struct SceneActorChaseTerrainCell *)SceneActorTerrain_GetCell(
                         actor->base.terrainCell))
                        ->flags;
        if ((cellFlags & 0x40) && actor->base.body.position.y + (s16)actor->base.heightOffset <=
                                      actor->base.groundHeight) {
            void *sink;
            if (SceneActorPlacement_IsUnavailable((void *)actor)) {
                node = node->next;
                actor = (void *)node->actor;
                continue;
            }
            sink = actor->base.sinkCallback;
            if (!sink) {
                SceneActorTerrain_InstallControl(actor, SceneActorReaction_SinkAndRemove, 0x16);
            } else {
                SceneActorTerrain_InstallControl(actor, sink, 0x16);
            }

            SceneActorTerrain_EnterState(actor, 3, 0, 0);
            MonsterInit_SetStatusFlags(actor, 0x20000);
        } else if ((cellFlags & 0x80) && MonsterInit_TestWorldFlags(0x10)) {
            struct ContactCollider *collider = &actor->base.colliderA;
            if (actor->base.colliderA.responseValue) {
                node = node->next;
                actor = (void *)node->actor;
                continue;
            }
            SceneActorTerrain_GetCenter(actor->base.terrainCell, &center);
            SceneActorTerrain_Subtract(&delta, &center, &actor->base.body.position);
            actor->base.reactionDirection = Math_Atan2(delta.x, delta.z) >> 5;
            actor->base.hitOwners = 0x20;
            collider->cost = MonsterInit_GetHealth(actor);
            actor->base.reactionSoundVariant = MonsterInit_GetHealth(actor);
            collider->responseParameter = 0x28;
            actor->base.hitAttributes = 0x200;
            actor->base.field2E6 = 1;
            MonsterInit_SetBehaviorFlags(actor, 0x2000);
        }
        node = node->next;
        actor = (void *)node->actor;
    }
}

struct SceneActorRegistryManager *SceneActorRegistry_GetManager(void)
{
    return gSceneActorRegistryManager;
}

void EffectQuad_StartRemoteEntry(u32, const void *);

void SceneActorScript_DispatchKindPayload(void *input)
{
    struct SceneActorBody *descriptor = input;
    u32 kind;
    if (!SceneActorControl_TestFlags(gEngineFlags, 0x4800))
        return;
    switch (descriptor->mode) {
        case 0:
            kind = 0;
            break;
        case 1:
            kind = 1;
            break;
        case 2:
            kind = 2;
            break;
        case 3:
            kind = 3;
            break;
        default:
            return;
    }
    EffectQuad_StartRemoteEntry(kind, &descriptor->position);
}
