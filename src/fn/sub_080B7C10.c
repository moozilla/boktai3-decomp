#include "global.h"
#include "scene_actor_chase_reaction.h"
#include "scene_actor_init_actor.h"

struct SkeletonSelectionContext {
    struct SceneActorRegistryResetView base;
    u8 unknown6EC[0x17];
    u8 attempts;
    u8 unknown704[4];
    u32 nearDistance, farDistance;
};
extern const u8 gUnk_0824E250[];
void SceneActorControl_OnIdle(u8 *);
void SceneActorReaction_ResetResponseState(void *, const void *, u32);
u32 Skeleton_TrySelectRetreatCell(struct SceneActorRegistryResetView *, u32);

static inline void Skeleton_CountDistanceSelection(struct SkeletonSelectionContext *context)
{
    u8 count = context->attempts;
    ++count;
    context->attempts = count;
}

static inline u8 Skeleton_TargetHeading(struct SceneActorRegistryResetView *actor)
{
    return actor->tracking.heading;
}

static inline u8 Skeleton_FaceAwayFromTarget(struct SceneActorRegistryResetView *actor)
{
    u8 priorDirection = MonsterInit_GetDirection(actor);
    actor->body.direction = Skeleton_TargetHeading(actor) + 0x80;
    return priorDirection;
}

static inline u8 Skeleton_TryRetreatFromTarget(struct SceneActorRegistryResetView *actor)
{
    u8 priorDirection = Skeleton_FaceAwayFromTarget(actor);
    return Skeleton_TrySelectRetreatCell(actor, priorDirection);
}

static inline struct SkeletonSelectionContext *
Skeleton_SelectionContext(struct SceneActorRegistryResetView *actor)
{
    return actor->context;
}

void Skeleton_SelectDistanceReaction(struct SkeletonSelectionContext *actor)
{
    struct SkeletonSelectionContext *context = Skeleton_SelectionContext(&actor->base);
    struct SceneActorChaseGroup *group = &actor->base.group;
    SceneActorReaction_ResetResponseState(&group->saved, gUnk_0824E250, 0x4F5);
    Skeleton_CountDistanceSelection(context);
    if (actor->base.tracking.distance < context->nearDistance) {
        if (!Skeleton_TryRetreatFromTarget(&actor->base))
            return;
        SceneActorChase_SetTypedControl(&actor->base, SceneActorControl_OnIdle, 0x10);
        SceneActorChase_SetPhase(&actor->base, 0, 15);
    } else if (actor->base.tracking.distance > context->farDistance) {
        if (!actor->base.tracking.target)
            SceneActorChase_TrackPlayer(&actor->base);
        if (SceneActorChase_Arrive(&actor->base)) {
            SceneActorChase_SetTypedControl(&actor->base, group->common.gate.control, 0x1A);
            SceneActorChase_SetPhase(&actor->base, 0, 12);
        }
    } else {
        if (!actor->base.tracking.target)
            SceneActorChase_TrackPlayer(&actor->base);
        if (SceneActorChase_Arrive(&actor->base)) {
            SceneActorChase_SetTypedControl(&actor->base, group->common.gate.control, 0x1A);
            SceneActorChase_SetPhase(&actor->base, 0, 12);
        }
    }
}
