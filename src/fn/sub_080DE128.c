#include "dog.h"
#include "scene_actor_chase.h"
#include "rotation_phase.h"
// confidence=low: chooses response 0x75/0x82/0x87 from target range, heading, and action timers.
#define FIELD(p, n, T) (*(T *)((u8 *)(p) + (n)))
extern const u8 gUnk_0824E2A8[];
void Dog_SelectRetreatOrChase(void *);
static inline u8 Dog_HasAwarenessFlag(struct Dog *context, u32 mask)
{
    u32 value = context->flags;
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 ActorFlag(struct ChaseActor *actor, u32 mask)
{
    return actor->base.flags2D4 & mask;
}
static inline void Response(struct ChaseActor *actor, u8 *nav, u32 behavior, u32 response)
{
    Chase_SetState(actor, 0, 2, behavior);
    Chase_SetCtl(actor, (void *)SceneActorControl_OnIdle, 0x10);
    SceneActorReaction_ResetResponseState(nav + 0x8C, gUnk_0824E2A8, response);
}
void Dog_SelectTargetAction(struct ChaseActor *actor)
{
    struct Dog *context = actor->base.context;
    u8 *nav = (u8 *)&actor->group.common;
    if (ActorFlag(actor, 0x100))
        return;
    if (actor->base.tracking.distance < actor->base.tracking.range) {
        struct ContactPosition delta;
        s32 angle;
        {
            struct ContactPosition *out = &delta;
            struct ContactPosition *from = &actor->base.body.position;
            struct ContactPosition *to =
                &((struct ChaseTarget *)actor->base.tracking.target)->position;
            SceneActorTerrain_Subtract(out, to, from);
        }
        angle =
            RotationPhase_HeadingDistance(Math_Atan2(delta.x, delta.z), actor->base.body.direction);
        if (angle <= 23) {
            u32 mode = FIELD(actor, 0x1E6, u16);
            if (mode == 0 || mode == 2) {
                if (actor->base.tracking.distance < context->speed && !context->nearTimer) {
                    Response(actor, nav, 0, 0x75);
                    return;
                }

                if (Dog_HasAwarenessFlag(context, 8))
                    return;
                goto move;
            } else {
                if (actor->base.tracking.distance < context->speed && !context->nearTimer) {
                    Response(actor, nav, 0, 0x82);
                    return;
                }
                if (actor->base.tracking.distance < context->alternateSpeed && angle <= 5 &&
                    !context->farTimer) {
                    Response(actor, nav, 1, 0x87);
                    return;
                }
                if (Dog_HasAwarenessFlag(context, 8))
                    return;
                goto move;
            }
        }
        if (!Dog_HasAwarenessFlag(context, 8)) {
        move:
            Dog_SelectRetreatOrChase(actor);
        }
    } else {
        if (!Dog_HasAwarenessFlag(context, 8))
            Dog_SelectRetreatOrChase(actor);
    }
}
