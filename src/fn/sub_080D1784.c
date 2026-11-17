#include "scene_actor_chase.h"
struct RecoverySequence {
    u8 pad[7];
    u8 mode;
    u16 frame;
    u8 padA[4];
    u16 timer;
};
static inline void SaveDelay(struct SceneActorReactionCommand *saved, void *target, u32 behavior,
                             u32 delay)
{
    saved->duration = delay;
    saved->payload = target;
    saved->first = 0;
    saved->second = 0;
    saved->kind = behavior;
}
static inline void ClearFlags(u16 *field, u32 mask)
{
    *field = *field & ~mask;
}
static inline u8 TryRoute(struct ChaseActor *actor, struct SceneActorChaseGroup *group)
{
    void *route = group->route;
    struct ContactPosition *position = &actor->base.body.position;
    /* Allocation-only: share the signed destination offset and motion flag. */
    s16 mask = (u8 *)&group->destination - (u8 *)group;
    struct ContactPosition *target;
    s32 result;
    target = *(struct ContactPosition **)((u8 *)group + mask);
    result = Nav_IsPositionInMesh(route, position, target);
    if (!result) {
        ClearFlags(&actor->base.motionFlags, mask);
        return 0;
    }
    result = Terrain_ApplyBlocked(route, position, target);
    if (result) {
        if (result != 2)
            Memory_Clear(&actor->base.body.motion, 8);
        return 1;
    }
    ClearFlags(&actor->base.motionFlags, mask);
    return 0;
}
struct RecoveryCounter {
    s16 value : 16;
};
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
// confidence=low: restarts the saved route after a timed animation and waits before restoring control.
void Mummy_ResumeSavedRouteAfterAnimation(struct ChaseActor *actor, s32 frame)
{
    void *body;
    struct SceneActorChaseGroup *group = &actor->group;
    struct RecoverySequence *sequence;
    body = &actor->base.body;
    sequence =
        (void *)((struct SceneActorAnimationSequenceDrawable *)actor->base.drawable)->sequence;
    if (SceneActorReaction_TakePrepared(actor)) {
        struct SceneActorRegistryControl *reaction = &group->common.control;
        {
            u32 *flags = &actor->base.behaviorFlags;
            u32 mask = ~0x141C0;
            *flags &= mask;
        }
        reaction->counter = 0;
        reaction->field14 = 0;
        reaction->timer = 0;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 7);
        actor->base.restart = 0;
    }
    if (frame <= 0x3B) {
        sequence->frame = 0;
        sequence->timer = 0;
    }
    if (actor->base.restart) {
        ((struct RecoveryCounter *)&actor->base.attachmentTimer)->value &= 0;
        actor->base.stepDone = 1;
        SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
        Chase_SetCtl(actor, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
        Chase_SetState(actor, 3, 1, 0);
        {
            struct SceneActorRegistryRoute *route = &group->homeRoute;
            SceneBody_StartRoute(body, route, group->homeRoute.routeId, group->homeRoute.parameterB,
                                 group->homeRoute.pointId);
            Chase_SnapToRoute(group->destination, (void *)route);
        }
        if (!TryRoute(actor, group)) {
            Chase_SetCtl(actor, (void *)SceneActorControl_OnIdle, 0);
            Chase_SetState(actor, 3, 0, 0);
        } else {
            group->destination->pad = 1;
            {
                u32 mask = 0x10;
                actor->base.behaviorFlags |= mask;
            }
            SaveDelay((void *)&group->saved, actor->base.onAlternateEntry, 7, 0x3C);
        }
    }
    /* Allocation-only: retain PRE expression order before these folds die. */
    {
        u32 folded = (u32)frame;
        folded += folded >> 3;
        folded += folded >> 5;
    }
}
