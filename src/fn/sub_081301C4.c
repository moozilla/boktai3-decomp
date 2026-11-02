#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_straight_route.h"
#include "scene_actor_attachment.h"

static inline void SetState(struct SceneActorRegistryResetView *actor, u8 s, u8 p, u8 b)
{
    actor->state2A8 = s;
    actor->phase2A9 = p;
    actor->behavior2AA = b;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *cb, u8 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = cb;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
// confidence=low: timed movement recovery restores a route and saves a delayed control.
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void SceneActorAttachment_ReplaceResponseSlot(void *, u32, u32, u32);
struct DelayedReactionControl {
    u8 state, phase, behavior, unknown3;
    u32 delay;
    void *callback;
};
struct ReactionSavedControl {
    struct SceneActorChaseSavedControl previous;
    struct DelayedReactionControl next;
};
static inline void SaveControl(struct SceneActorChaseSavedControl *record, void *callback,
                               u8 behavior)
{
    struct ReactionSavedControl *saved = (void *)record;
    saved->next.delay = 30;
    saved->next.callback = callback;
    saved->next.state = 0;
    saved->next.phase = 0;
    saved->next.behavior = behavior;
}
void SceneActorReaction_UpdateRouteRecovery(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = &actor->group;
    u8 result;
    u32 *savedFlags;
    if (actor->field2B1)
        actor->field2B1 = 0;
    if ((u8)Callback_InvokeActor(actor, actor->contactQuery.opaque))
        return;
    result = sub_0812F3CC(actor);
    if (result)
        return;
    if ((s32)actor->counter2AC > 60) {
        struct ContactPosition target;
        {
            u32 *flags = &actor->flags2CC;
            u32 mask = ~0x100;
            *flags &= mask;
        }
        SceneActorAttachment_ReplaceResponseSlot(actor, 0, 2, actor->category);
        SetState(actor, 3, 1, 0);
        SetControl(actor, SceneActorReaction_RestoreAfterDelay, 14);
        {
            u32 *flags = &actor->flags2CC;
            u32 mask = 0x10;
            *flags |= mask;
        }
        {
            struct ContactPosition *p = &target;
            struct SceneActorChaseRoutePoint *route = group->homeRoute.point;
            p->x = route->x;
            p->z = route->z;
            {
                s32 x = p->x >> 8;
                s32 z = p->z >> 8;

                savedFlags = &actor->flags2CC;
                p->y = SceneActorTerrain_GetHeight(Chase_CellIndex(x, z), p);
            }
        }
        target.y = actor->body.position.y;
        SceneActorTerrain_RequestPath(actor, group->route, &actor->body.position, &target);
        group->destination->pad = 1;
        if (!(u8)SceneActorAttachment_TestPositionWindow(actor, &target, 32, 32)) {
            {
                u32 *flags = savedFlags;
                u32 mask = ~0xc0;
                *flags &= mask;
            }
            SaveControl(&group->saved, actor->entryTarget228, 7);
        } else {
            {
                u32 *flags = savedFlags;
                u32 mask = ~0xd0;
                *flags &= mask;
            }
            SaveControl(&group->saved, actor->savedTarget, 0);
        }
        actor->field2C2 = 64;
    }
    actor->counter2AC++;
}