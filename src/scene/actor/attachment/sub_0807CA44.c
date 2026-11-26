#include "scene/collision/scene_actor_contacts.h"
#include "scene/collision/scene_actor_delayed_response.h"
#include "scene/motion/scene_actor_terrain.h"
#include "scene/motion/scene_actor_terrain_reaction.h"
#include "scene/actor/scene_actor_reaction.h"
#include "engine/engine.h"
#include "scene/motion/scene_actor_position_search.h"
#include "scene/motion/scene_actor_chase_reaction.h"

void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);

static inline s32 HeightAt(struct ContactPosition *position)
{
    return SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(position), position);
}

static inline void *ReadNavigationShape(struct SceneActorChaseGroup *const *group)
{
    return (*group)->route;
}

static inline u8 MoveTo(struct ChaseActor *owner, struct ChaseActor *actor,
                        struct SceneActorChaseGroup *group, struct ContactPosition *destination)
{
    void *shape = ReadNavigationShape(&group);
    struct ContactPosition *position = &actor->base.body.position;
    return SceneActorChase_TryRoute(owner, shape, position, destination);
}

static inline void SetCoordinates(struct ContactPosition *out, struct SceneActorChaseGroup *group)
{
    struct SceneActorChaseRoutePoint *point = group->homeRoute.point;
    out->x = point->x;
    out->z = point->z;
    out->y = HeightAt(out);
}

static inline u8 MoveToSource(struct ChaseActor *owner, struct ChaseActor *actor,
                              struct SceneActorChaseGroup *group,
                              struct SceneActorContactNode *source)
{
    void *shape = group->route;
    struct ContactPosition *position = &actor->base.body.position;
    struct SceneActorPositionSource *target = source->object;
    struct ContactPosition *destination = &target->position;
    return SceneActorChase_TryRoute(owner, shape, position, destination);
}

void SceneActorAttachment_BeginReturnResponse(struct ChaseActor *actor)
{
    struct ChaseActor *owner = actor;
    struct SceneActorChaseGroup *group = &actor->group;
    struct ContactPosition position;
    struct ContactPosition *destination;
    struct SceneActorRegistryRoute *source;
    SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
    destination = actor->group.destination;
    source = &actor->group.homeRoute;
    destination->x = source->point->x;
    destination->z = source->point->z;
    destination->y = HeightAt(destination);
    if (!MoveToSource(owner, actor, group, group->contactNode)) {
        SetCoordinates(&position, group);
        MoveTo(owner, actor, group, &position);
        SceneActorTerrain_InstallControl(actor, actor->base.onAlternateEntry, 1);
        SceneActorTerrain_EnterState(actor, 0, 0, 2);
    } else {
        SceneActorAttachment_ReplaceResponseSlot(owner, 0, 1, actor->base.category);
        SceneActorTerrain_EnterState(actor, 3, 1, 0);
        SceneActorTerrain_InstallControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        SceneActorDelay_Prepare((void *)&group->saved, actor->base.onAlternateEntry, 7, 30);
        SceneActorChase_SetBehaviorFlags(actor, 0x10);
        SceneActorChase_SetMotionFlags(actor, 0x1000);
        group->destination->pad = 0;
    }
}
