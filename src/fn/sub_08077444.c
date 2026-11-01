#include "scene_actor_placement.h"
#include "scene_actor_status.h"
#include "scene_actor_terrain_reaction.h"
static inline void SceneActorPlacement_ResetControl(struct SceneActorRegistryResetView *actor,
                                                    struct SceneActorRegistryControl *control)
{
    if (ActorStatus_BehaviorFlags(actor, 0x200000)) {
        ActorStatus_ClearInteraction(actor, control);
    }
}
static inline void SceneActorPlacement_ClearColliderFlags(struct ContactCollider *collider,
                                                          u16 flags)
{
    u32 value = collider->flags;
    u32 mask = (u16)~flags;
    mask &= value;
    collider->flags = mask;
}
void SceneActorPlacement_ResetToPathPoint(struct SceneActorRegistryActor *object, u32 pathId, u32 pointId, u32 direction)
{
    struct SceneActorRegistryResetView *actor = (void *)object;
    struct SceneActorRegistryControlGroup *group = &actor->group.common;
    struct SceneActorBody *body = &actor->body;
    u32 *flags;
    u32 enabled = 1;
    flags = &actor->flags2CC;
    if (*flags & enabled) {
        struct ContactPosition *position;
        SceneActorChase_ClearWordFlags(flags, 3);
        actor->group.homeRoute.currentPoint = actor->group.homeRoute.pointId;
        position = &actor->body.position;
        Path_GetPointPositionById((u16 *)position, pathId, pointId);
        actor->terrainCell = SceneActorTerrain_GetCellIndex(position);
        body->position.y = SceneActorTerrain_GetHeight(actor->terrainCell, &actor->body.position);
        if (!actor->drawableKind)
            ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->position =
                body->position;
        else
            ((struct SceneActorRegistryTileDrawable *)actor->drawable)->position = body->position;
        body->direction = direction;
        SceneActorPlacement_ResetControl(actor, &group->control);
        SceneActorPlacement_ClearColliderFlags(&actor->colliderA, 4);
        ActorStatus_SetDrawableFlags(actor, 2);
        ActorStatus_ClearDrawableFlags(actor, 1);
        SceneActorTerrain_EnterState(actor, 3, 0, 0);
        SceneActorTerrain_InstallControl(actor, actor->field220, 0xC);
    }
}
