#include "global.h"
#include "skeleton.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_init_actor.h"

extern const u8 gUnk_0824E250[];
void SceneActorControl_OnIdle(u8 *);
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
u32 Skeleton_TrySelectRetreatCell(struct ChaseActor *, u32);

static inline void Skeleton_CountDistanceSelection(struct Skeleton *context)
{
    u8 count = context->attempts;
    ++count;
    context->attempts = count;
}

static inline u8 Skeleton_TargetHeading(struct ChaseActor *actor)
{
    return actor->base.tracking.heading;
}

static inline u8 Skeleton_FaceAwayFromTarget(struct ChaseActor *actor)
{
    u8 priorDirection = MonsterInit_GetDirection(actor);
    actor->base.body.direction = Skeleton_TargetHeading(actor) + 0x80;
    return priorDirection;
}

static inline u8 Skeleton_TryRetreatFromTarget(struct ChaseActor *actor)
{
    u8 priorDirection = Skeleton_FaceAwayFromTarget(actor);
    return Skeleton_TrySelectRetreatCell(actor, priorDirection);
}

static inline struct Skeleton *Skeleton_SelectionContext(struct ChaseActor *actor)
{
    return actor->base.context;
}

void Skeleton_SelectDistanceReaction(struct Skeleton *actor)
{
    struct Skeleton *context = Skeleton_SelectionContext(&actor->chase);
    struct SceneActorChaseGroup *group = &actor->chase.group;
    SceneActorReaction_ResetResponseState(&group->saved, gUnk_0824E250, 0x4F5);
    Skeleton_CountDistanceSelection(context);
    if (actor->chase.base.tracking.distance < context->nearDistance) {
        if (!Skeleton_TryRetreatFromTarget(&actor->chase))
            return;
        SceneActorChase_SetTypedControl(&actor->chase, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetPhase(&actor->chase, 0, 15);
    } else if (actor->chase.base.tracking.distance > context->farDistance) {
        if (!actor->chase.base.tracking.target)
            SceneActorChase_TrackPlayer(&actor->chase);
        if (SceneActorChase_Arrive(&actor->chase)) {
            SceneActorChase_SetTypedControl(&actor->chase, group->common.gate.control, 0x1A);
            SceneActorChase_SetPhase(&actor->chase, 0, 12);
        }
    } else {
        if (!actor->chase.base.tracking.target)
            SceneActorChase_TrackPlayer(&actor->chase);
        if (SceneActorChase_Arrive(&actor->chase)) {
            SceneActorChase_SetTypedControl(&actor->chase, group->common.gate.control, 0x1A);
            SceneActorChase_SetPhase(&actor->chase, 0, 12);
        }
    }
}
